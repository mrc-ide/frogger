#pragma once

#include <Rcpp.h>

#include "state_space_mixer.hpp"
#include "tensor_types.hpp"

namespace leapfrog {

template <typename T>
T* r_data(SEXP x) {
  static_assert(sizeof(T) == 0, "Only specializations of r_data can be used");
}

template <>
double* r_data(SEXP x) {
  return REAL(x);
}

template <>
int* r_data(SEXP x) {
  return INTEGER(x);
}

template<typename T, typename... Args>
auto parse_data(const Rcpp::List data, const std::string& key, Args... dims) {
  constexpr std::size_t rank = sizeof...(dims);
  Eigen::array<int, rank> dimensions{ static_cast<int>(dims)... };

  int length = std::accumulate(dimensions.begin(), dimensions.end(), 1, std::multiplies<int>());
  SEXP array_data = data[key];
  // In cases where the input data has project years we might not use all of it model fit
  // So we can take create a Map over a smaller slice of the data
  // As long as this is true we can be confident we're not referencing invalid memory
  if (LENGTH(array_data) < length) {
    Rcpp::stop("Invalid size of data for '%s', expected %d got %d",
               key,
               length,
               LENGTH(array_data));
  }

  return Eigen::TensorMap<Eigen::Tensor<T, rank>>(r_data<T>(array_data), static_cast<int>(dims)...);
}

// ****************************** OPTIONS ****************************** //

template<typename real_type>
struct Opts {
  int hts_per_year;
  double dt;
  const int p_idx_fertility_first;
  const int p_fertility_age_groups;
  const int p_idx_hiv_first_adult;
  const int adult_incidence_first_age_group;
  const int pAG_INCIDPOP;
  const int ts_art_start;
  const int hIDX_15PLUS;
  const int proj_period_int;

  Opts(
    int hts_per_year,
    int ts_art_start,
    int proj_period_int
  ):
    hts_per_year(hts_per_year),
    dt(1.0 / hts_per_year),
    p_idx_fertility_first(15),
    p_fertility_age_groups(35),
    p_idx_hiv_first_adult(15),
    adult_incidence_first_age_group(15),
    pAG_INCIDPOP(35),
    ts_art_start(ts_art_start),
    hIDX_15PLUS(0),
    proj_period_int(proj_period_int) {}
};

template<typename real_type, MV ModelVariant>
struct DpConfig {
  using SS = SSMixed<ModelVariant>;

  struct Pars {
    TM2<real_type> base_pop;
    TM3<real_type> survival_probability;
    TM3<real_type> net_migration;
    TM2<real_type> age_specific_fertility_rate;
    TM2<real_type> births_sex_prop;
  };

  static Pars get_pars(
    const Rcpp::List &data,
    const Opts<real_type> &opts,
    const int proj_years
  ) {
    return {
      .base_pop = parse_data<real_type>(data, "basepop", SS::pAG, SS::NS),
      .survival_probability = parse_data<real_type>(data, "Sx", SS::pAG + 1, SS::NS, proj_years),
      .net_migration = parse_data<real_type>(data, "netmigr_adj", SS::pAG, SS::NS, proj_years),
      .age_specific_fertility_rate = parse_data<real_type>(data, "asfr", opts.p_fertility_age_groups, proj_years),
      .births_sex_prop = parse_data<real_type>(data, "births_sex_prop", SS::NS, proj_years)
    };
  };

  struct Intermediate {
    TFS<real_type, SS::pAG, SS::NS> migration_rate;

    Intermediate() {};

    void reset() {
      migration_rate.setZero();
    };
  };

  struct State {
    TFS<real_type, SS::pAG, SS::NS> p_total_pop;
    TFS<real_type, SS::pAG, SS::NS> p_total_pop_natural_deaths;
    real_type births;

    State() {
      reset();
    };

    State(const State& initial_state):
      p_total_pop(initial_state.p_total_pop),
      p_total_pop_natural_deaths(initial_state.p_total_pop_natural_deaths),
      births(initial_state.births)
    {};

    void reset() {
      p_total_pop.setZero();
      p_total_pop_natural_deaths.setZero();
      births = 0;
    };
  };

  struct OutputState {
    T3<real_type> p_total_pop;
    T3<real_type> p_total_pop_natural_deaths;
    T1<real_type> births;

    OutputState(int output_years):
      p_total_pop(SS::pAG, SS::NS, output_years),
      p_total_pop_natural_deaths(SS::pAG, SS::NS, output_years),
      births(output_years)
    {
      p_total_pop.setZero();
      p_total_pop_natural_deaths.setZero();
      births.setZero();
    };

    void save_state(const size_t i, const State &state) {
      p_total_pop.chip(i, p_total_pop.NumDimensions - 1) = state.p_total_pop;
      p_total_pop_natural_deaths.chip(i, p_total_pop_natural_deaths.NumDimensions - 1) = state.p_total_pop_natural_deaths;
      births(i) = state.births;
    };
  };

  static constexpr int output_count = 3;
  static int get_build_output_size(int prev_size) {
    return prev_size + output_count;
  };

  static int build_output(
    Rcpp::List& ret,
    Rcpp::CharacterVector& names,
    int index,
    const OutputState& state,
    const size_t& output_years
  ) {
    Rcpp::NumericVector r_p_total_pop(SS::pAG * SS::NS * output_years);
    r_p_total_pop.attr("dim") = Rcpp::IntegerVector::create(SS::pAG, SS::NS, output_years);
    std::copy_n(state.p_total_pop.data(), state.p_total_pop.size(), REAL(r_p_total_pop));
    names[index + 0] = "p_total_pop";
    ret[index + 0] = r_p_total_pop;
    Rcpp::NumericVector r_p_total_pop_natural_deaths(SS::pAG * SS::NS * output_years);
    r_p_total_pop_natural_deaths.attr("dim") = Rcpp::IntegerVector::create(SS::pAG, SS::NS, output_years);
    std::copy_n(state.p_total_pop_natural_deaths.data(), state.p_total_pop_natural_deaths.size(), REAL(r_p_total_pop_natural_deaths));
    names[index + 1] = "p_total_pop_natural_deaths";
    ret[index + 1] = r_p_total_pop_natural_deaths;
    Rcpp::NumericVector r_births(output_years);
    r_births.attr("dim") = Rcpp::IntegerVector::create(output_years);
    std::copy_n(state.births.data(), state.births.size(), REAL(r_births));
    names[index + 2] = "births";
    ret[index + 2] = r_births;
    return index + output_count;
  };
};
template<typename real_type, MV ModelVariant>
struct HaConfig {
  using SS = SSMixed<ModelVariant>;

  struct Pars {
    TM1<real_type> total_rate;
    TM3<real_type> relative_risk_age;
    TM1<real_type> relative_risk_sex;
    TM3<real_type> cd4_mortality;
    TM3<real_type> cd4_progression;
    TM3<real_type> cd4_initial_distribution;
    int scale_cd4_mortality;
    TM1<int> idx_hm_elig;
    TM4<real_type> mortality;
    TM2<real_type> mortality_time_rate_ratio;
    int dropout_recover_cd4;
    TM1<real_type> dropout_rate;
    TM2<real_type> adults_on_art;
    TM2<int> adults_on_art_is_percent;
    TM1<real_type> h_art_stage_dur;
    real_type initiation_mortality_weight;
  };

  static Pars get_pars(
    const Rcpp::List &data,
    const Opts<real_type> &opts,
    const int proj_years
  ) {
    return {
      .total_rate = parse_data<real_type>(data, "incidinput", proj_years),
      .relative_risk_age = parse_data<real_type>(data, "incrr_age", SS::pAG - opts.p_idx_hiv_first_adult, SS::NS, proj_years),
      .relative_risk_sex = parse_data<real_type>(data, "incrr_sex", proj_years),
      .cd4_mortality = parse_data<real_type>(data, "cd4_mort", SS::hDS, SS::hAG, SS::NS),
      .cd4_progression = parse_data<real_type>(data, "cd4_prog", SS::hDS - 1, SS::hAG, SS::NS),
      .cd4_initial_distribution = parse_data<real_type>(data, "cd4_initdist", SS::hDS, SS::hAG, SS::NS),
      .scale_cd4_mortality = Rcpp::as<int>(data["scale_cd4_mort"]),
      .idx_hm_elig = parse_data<int>(data, "artcd4elig_idx", proj_years),
      .mortality = parse_data<real_type>(data, "art_mort", SS::hTS, SS::hDS, SS::hAG, SS::NS),
      .mortality_time_rate_ratio = parse_data<real_type>(data, "artmx_timerr", SS::hTS, proj_years),
      .dropout_recover_cd4 = Rcpp::as<int>(data["art_dropout_recover_cd4"]),
      .dropout_rate = parse_data<real_type>(data, "art_dropout_rate", proj_years),
      .adults_on_art = parse_data<real_type>(data, "art15plus_num", SS::NS, proj_years),
      .adults_on_art_is_percent = parse_data<int>(data, "art15plus_isperc", SS::NS, proj_years),
      .h_art_stage_dur = parse_data<real_type>(data, "h_art_stage_dur", SS::hTS - 1),
      .initiation_mortality_weight = Rcpp::as<real_type>(data["art_alloc_mxweight"])
    };
  };

  struct Intermediate {
    TFS<real_type, SS::pAG, SS::NS> hiv_net_migration;
    TFS<real_type, SS::pAG> hiv_negative_pop;
    TFS<real_type, SS::NS> rate_sex;
    TFS<real_type, SS::NS> hiv_neg_aggregate;
    TFS<real_type, SS::pAG, SS::NS> p_infections_ts;
    TFS<real_type, SS::hAG, SS::NS> p_hiv_pop_coarse_ages;
    TFS<real_type, SS::hAG, SS::NS> hiv_age_up_prob;
    TFS<real_type, SS::hAG, SS::NS> p_hiv_deaths_age_sex;
    TFS<real_type, SS::hDS, SS::hAG, SS::NS> grad;
    TFS<real_type, SS::hTS, SS::hDS, SS::hAG, SS::NS> gradART;
    TFS<real_type, SS::hAG> hivpop_ha;
    TFS<real_type, SS::hDS, SS::hAG> artelig_hahm;
    TFS<real_type, SS::hDS> artelig_hm;
    TFS<real_type, SS::hDS> expect_mort_artelig_hm;
    TFS<real_type, SS::hDS> artinit_hm;
    real_type Xart_15plus;
    real_type Xartelig_15plus;
    real_type expect_mort_artelig15plus;
    real_type cd4mx_scale;
    real_type artpop_hahm;
    real_type deaths;
    real_type p_infections_a;
    real_type p_infections_ha;
    real_type deaths_art;
    real_type artnum_hts;
    real_type artcov_hts;
    real_type curr_coverage;
    real_type artinit_hts;
    real_type artinit_hahm;
    real_type hivqx_ha;
    real_type hivdeaths_a;
    real_type Xhivn_incagerr;
    int everARTelig_idx;
    int cd4elig_idx;
    int anyelig_idx;

    Intermediate() {};

    void reset() {
      hiv_net_migration.setZero();
      hiv_negative_pop.setZero();
      rate_sex.setZero();
      hiv_neg_aggregate.setZero();
      p_infections_ts.setZero();
      p_hiv_pop_coarse_ages.setZero();
      hiv_age_up_prob.setZero();
      p_hiv_deaths_age_sex.setZero();
      grad.setZero();
      gradART.setZero();
      hivpop_ha.setZero();
      artelig_hahm.setZero();
      artelig_hm.setZero();
      expect_mort_artelig_hm.setZero();
      artinit_hm.setZero();
      Xart_15plus = 0;
      Xartelig_15plus = 0;
      expect_mort_artelig15plus = 0;
      cd4mx_scale = 1;
      artpop_hahm = 0;
      deaths = 0;
      p_infections_a = 0;
      p_infections_ha = 0;
      deaths_art = 0;
      artnum_hts = 0;
      artcov_hts = 0;
      curr_coverage = 0;
      artinit_hts = 0;
      artinit_hahm = 0;
      hivqx_ha = 0;
      hivdeaths_a = 0;
      Xhivn_incagerr = 0;
      everARTelig_idx = 0;
      cd4elig_idx = 0;
      anyelig_idx = 0;
    };
  };

  struct State {
    TFS<real_type, SS::pAG, SS::NS> p_hiv_pop;
    TFS<real_type, SS::pAG, SS::NS> p_hiv_pop_natural_deaths;
    TFS<real_type, SS::hDS, SS::hAG, SS::NS> h_hiv_adult;
    TFS<real_type, SS::hTS, SS::hDS, SS::hAG, SS::NS> h_art_adult;
    TFS<real_type, SS::hDS, SS::hAG, SS::NS> h_hiv_deaths_no_art;
    TFS<real_type, SS::pAG, SS::NS> p_infections;
    TFS<real_type, SS::hTS, SS::hDS, SS::hAG, SS::NS> h_hiv_deaths_art;
    TFS<real_type, SS::hDS, SS::hAG, SS::NS> h_art_initiation;
    TFS<real_type, SS::pAG, SS::NS> p_hiv_deaths;

    State() {
      reset();
    };

    State(const State& initial_state):
      p_hiv_pop(initial_state.p_hiv_pop),
      p_hiv_pop_natural_deaths(initial_state.p_hiv_pop_natural_deaths),
      h_hiv_adult(initial_state.h_hiv_adult),
      h_art_adult(initial_state.h_art_adult),
      h_hiv_deaths_no_art(initial_state.h_hiv_deaths_no_art),
      p_infections(initial_state.p_infections),
      h_hiv_deaths_art(initial_state.h_hiv_deaths_art),
      h_art_initiation(initial_state.h_art_initiation),
      p_hiv_deaths(initial_state.p_hiv_deaths)
    {};

    void reset() {
      p_hiv_pop.setZero();
      p_hiv_pop_natural_deaths.setZero();
      h_hiv_adult.setZero();
      h_art_adult.setZero();
      h_hiv_deaths_no_art.setZero();
      p_infections.setZero();
      h_hiv_deaths_art.setZero();
      h_art_initiation.setZero();
      p_hiv_deaths.setZero();
    };
  };

  struct OutputState {
    T3<real_type> p_hiv_pop;
    T3<real_type> p_hiv_pop_natural_deaths;
    T4<real_type> h_hiv_adult;
    T5<real_type> h_art_adult;
    T4<real_type> h_hiv_deaths_no_art;
    T3<real_type> p_infections;
    T5<real_type> h_hiv_deaths_art;
    T4<real_type> h_art_initiation;
    T3<real_type> p_hiv_deaths;

    OutputState(int output_years):
      p_hiv_pop(SS::pAG, SS::NS, output_years),
      p_hiv_pop_natural_deaths(SS::pAG, SS::NS, output_years),
      h_hiv_adult(SS::hDS, SS::hAG, SS::NS, output_years),
      h_art_adult(SS::hTS, SS::hDS, SS::hAG, SS::NS, output_years),
      h_hiv_deaths_no_art(SS::hDS, SS::hAG, SS::NS, output_years),
      p_infections(SS::pAG, SS::NS, output_years),
      h_hiv_deaths_art(SS::hTS, SS::hDS, SS::hAG, SS::NS, output_years),
      h_art_initiation(SS::hDS, SS::hAG, SS::NS, output_years),
      p_hiv_deaths(SS::pAG, SS::NS, output_years)
    {
      p_hiv_pop.setZero();
      p_hiv_pop_natural_deaths.setZero();
      h_hiv_adult.setZero();
      h_art_adult.setZero();
      h_hiv_deaths_no_art.setZero();
      p_infections.setZero();
      h_hiv_deaths_art.setZero();
      h_art_initiation.setZero();
      p_hiv_deaths.setZero();
    };

    void save_state(const size_t i, const State &state) {
      p_hiv_pop.chip(i, p_hiv_pop.NumDimensions - 1) = state.p_hiv_pop;
      p_hiv_pop_natural_deaths.chip(i, p_hiv_pop_natural_deaths.NumDimensions - 1) = state.p_hiv_pop_natural_deaths;
      h_hiv_adult.chip(i, h_hiv_adult.NumDimensions - 1) = state.h_hiv_adult;
      h_art_adult.chip(i, h_art_adult.NumDimensions - 1) = state.h_art_adult;
      h_hiv_deaths_no_art.chip(i, h_hiv_deaths_no_art.NumDimensions - 1) = state.h_hiv_deaths_no_art;
      p_infections.chip(i, p_infections.NumDimensions - 1) = state.p_infections;
      h_hiv_deaths_art.chip(i, h_hiv_deaths_art.NumDimensions - 1) = state.h_hiv_deaths_art;
      h_art_initiation.chip(i, h_art_initiation.NumDimensions - 1) = state.h_art_initiation;
      p_hiv_deaths.chip(i, p_hiv_deaths.NumDimensions - 1) = state.p_hiv_deaths;
    };
  };

  static constexpr int output_count = 9;
  static int get_build_output_size(int prev_size) {
    return prev_size + output_count;
  };

  static int build_output(
    Rcpp::List& ret,
    Rcpp::CharacterVector& names,
    int index,
    const OutputState& state,
    const size_t& output_years
  ) {
    Rcpp::NumericVector r_p_hiv_pop(SS::pAG * SS::NS * output_years);
    r_p_hiv_pop.attr("dim") = Rcpp::IntegerVector::create(SS::pAG, SS::NS, output_years);
    std::copy_n(state.p_hiv_pop.data(), state.p_hiv_pop.size(), REAL(r_p_hiv_pop));
    names[index + 0] = "p_hiv_pop";
    ret[index + 0] = r_p_hiv_pop;
    Rcpp::NumericVector r_p_hiv_pop_natural_deaths(SS::pAG * SS::NS * output_years);
    r_p_hiv_pop_natural_deaths.attr("dim") = Rcpp::IntegerVector::create(SS::pAG, SS::NS, output_years);
    std::copy_n(state.p_hiv_pop_natural_deaths.data(), state.p_hiv_pop_natural_deaths.size(), REAL(r_p_hiv_pop_natural_deaths));
    names[index + 1] = "p_hiv_pop_natural_deaths";
    ret[index + 1] = r_p_hiv_pop_natural_deaths;
    Rcpp::NumericVector r_h_hiv_adult(SS::hDS * SS::hAG * SS::NS * output_years);
    r_h_hiv_adult.attr("dim") = Rcpp::IntegerVector::create(SS::hDS, SS::hAG, SS::NS, output_years);
    std::copy_n(state.h_hiv_adult.data(), state.h_hiv_adult.size(), REAL(r_h_hiv_adult));
    names[index + 2] = "h_hiv_adult";
    ret[index + 2] = r_h_hiv_adult;
    Rcpp::NumericVector r_h_art_adult(SS::hTS * SS::hDS * SS::hAG * SS::NS * output_years);
    r_h_art_adult.attr("dim") = Rcpp::IntegerVector::create(SS::hTS, SS::hDS, SS::hAG, SS::NS, output_years);
    std::copy_n(state.h_art_adult.data(), state.h_art_adult.size(), REAL(r_h_art_adult));
    names[index + 3] = "h_art_adult";
    ret[index + 3] = r_h_art_adult;
    Rcpp::NumericVector r_h_hiv_deaths_no_art(SS::hDS * SS::hAG * SS::NS * output_years);
    r_h_hiv_deaths_no_art.attr("dim") = Rcpp::IntegerVector::create(SS::hDS, SS::hAG, SS::NS, output_years);
    std::copy_n(state.h_hiv_deaths_no_art.data(), state.h_hiv_deaths_no_art.size(), REAL(r_h_hiv_deaths_no_art));
    names[index + 4] = "h_hiv_deaths_no_art";
    ret[index + 4] = r_h_hiv_deaths_no_art;
    Rcpp::NumericVector r_p_infections(SS::pAG * SS::NS * output_years);
    r_p_infections.attr("dim") = Rcpp::IntegerVector::create(SS::pAG, SS::NS, output_years);
    std::copy_n(state.p_infections.data(), state.p_infections.size(), REAL(r_p_infections));
    names[index + 5] = "p_infections";
    ret[index + 5] = r_p_infections;
    Rcpp::NumericVector r_h_hiv_deaths_art(SS::hTS * SS::hDS * SS::hAG * SS::NS * output_years);
    r_h_hiv_deaths_art.attr("dim") = Rcpp::IntegerVector::create(SS::hTS, SS::hDS, SS::hAG, SS::NS, output_years);
    std::copy_n(state.h_hiv_deaths_art.data(), state.h_hiv_deaths_art.size(), REAL(r_h_hiv_deaths_art));
    names[index + 6] = "h_hiv_deaths_art";
    ret[index + 6] = r_h_hiv_deaths_art;
    Rcpp::NumericVector r_h_art_initiation(SS::hDS * SS::hAG * SS::NS * output_years);
    r_h_art_initiation.attr("dim") = Rcpp::IntegerVector::create(SS::hDS, SS::hAG, SS::NS, output_years);
    std::copy_n(state.h_art_initiation.data(), state.h_art_initiation.size(), REAL(r_h_art_initiation));
    names[index + 7] = "h_art_initiation";
    ret[index + 7] = r_h_art_initiation;
    Rcpp::NumericVector r_p_hiv_deaths(SS::pAG * SS::NS * output_years);
    r_p_hiv_deaths.attr("dim") = Rcpp::IntegerVector::create(SS::pAG, SS::NS, output_years);
    std::copy_n(state.p_hiv_deaths.data(), state.p_hiv_deaths.size(), REAL(r_p_hiv_deaths));
    names[index + 8] = "p_hiv_deaths";
    ret[index + 8] = r_p_hiv_deaths;
    return index + output_count;
  };
};
template<typename real_type, MV ModelVariant>
struct HcConfig {
  using SS = SSMixed<ModelVariant>;

  struct Pars {
    TM1<real_type> hc_nosocomial;
    TM1<real_type> hc1_cd4_dist;
    TM2<real_type> hc_cd4_transition;
    TM3<real_type> hc1_cd4_mort;
    TM3<real_type> hc2_cd4_mort;
    TM3<real_type> hc1_cd4_prog;
    TM3<real_type> hc2_cd4_prog;
    TM1<real_type> ctx_val;
    TM1<int> hc_art_elig_age;
    TM2<real_type> hc_art_elig_cd4;
    TM3<real_type> hc_art_mort_rr;
    TM3<real_type> hc1_art_mort;
    TM3<real_type> hc2_art_mort;
    TM1<int> hc_art_isperc;
    TM2<real_type> hc_art_val;
    TM2<real_type> hc_art_init_dist;
    TM2<real_type> adult_cd4_dist;
    TM1<real_type> fert_mult_by_age;
    TM1<real_type> fert_mult_off_art;
    TM1<real_type> fert_mult_on_art;
    TM1<real_type> total_fertility_rate;
    TM2<real_type> PMTCT;
    TM2<real_type> vertical_transmission_rate;
    TM3<real_type> PMTCT_transmission_rate;
    TM2<real_type> PMTCT_dropout;
    TM1<int> PMTCT_input_is_percent;
    TM2<real_type> breastfeeding_duration_art;
    TM2<real_type> breastfeeding_duration_no_art;
    TM1<real_type> mat_hiv_births;
    TM1<int> mat_prev_input;
    TM1<real_type> prop_lt200;
    TM1<real_type> prop_gte350;
    TM1<real_type> incrate;
    TM1<int> ctx_val_is_percent;
    TM1<int> hc_art_is_age_spec;
    TM1<real_type> hc_age_coarse;
    TM2<real_type> abortion;
    TM1<real_type> patients_reallocated;
    TM1<real_type> hc_art_ltfu;
    TM1<int> hc_age_coarse_cd4;
    TM2<real_type> adult_female_infections;
    TM2<real_type> adult_female_hivnpop;
    TM1<real_type> total_births;
    real_type ctx_effect;
    real_type local_adj_factor;
  };

  static Pars get_pars(
    const Rcpp::List &data,
    const Opts<real_type> &opts,
    const int proj_years
  ) {
    return {
      .hc_nosocomial = parse_data<real_type>(data, "paed_incid_input", proj_years),
      .hc1_cd4_dist = parse_data<real_type>(data, "paed_cd4_dist", SS::hc2DS),
      .hc_cd4_transition = parse_data<real_type>(data, "paed_cd4_transition", SS::hc2DS, SS::hc1DS),
      .hc1_cd4_mort = parse_data<real_type>(data, "paed_cd4_mort", SS::hc1DS, SS::hcTT, SS::hc1AG),
      .hc2_cd4_mort = parse_data<real_type>(data, "adol_cd4_mort", SS::hc2DS, SS::hcTT, SS::hc2AG),
      .hc1_cd4_prog = parse_data<real_type>(data, "paed_cd4_prog", SS::hc1DS, SS::hc1AG_c, SS::NS),
      .hc2_cd4_prog = parse_data<real_type>(data, "adol_cd4_prog", SS::hc2DS, SS::hc2AG_c, SS::NS),
      .ctx_val = parse_data<real_type>(data, "ctx_val", proj_years),
      .hc_art_elig_age = parse_data<int>(data, "paed_art_elig_age", proj_years),
      .hc_art_elig_cd4 = parse_data<real_type>(data, "paed_art_elig_cd4", opts.p_idx_hiv_first_adult, proj_years),
      .hc_art_mort_rr = parse_data<real_type>(data, "mort_art_rr", SS::hTS, opts.p_idx_hiv_first_adult, proj_years),
      .hc1_art_mort = parse_data<real_type>(data, "paed_art_mort", SS::hc1DS, SS::hTS, SS::hc1AG),
      .hc2_art_mort = parse_data<real_type>(data, "adol_art_mort", SS::hc2DS, SS::hTS, SS::hc2AG),
      .hc_art_isperc = parse_data<int>(data, "artpaeds_isperc", proj_years),
      .hc_art_val = parse_data<real_type>(data, "paed_art_val", SS::hcAG_coarse, proj_years),
      .hc_art_init_dist = parse_data<real_type>(data, "init_art_dist", opts.p_idx_hiv_first_adult, proj_years),
      .adult_cd4_dist = parse_data<real_type>(data, "adult_cd4_dist", SS::hDS, SS::hc2DS),
      .fert_mult_by_age = parse_data<real_type>(data, "fert_mult_by_age", opts.p_fertility_age_groups),
      .fert_mult_off_art = parse_data<real_type>(data, "fert_mult_offart", SS::hDS),
      .fert_mult_on_art = parse_data<real_type>(data, "fert_mult_onart", opts.p_fertility_age_groups),
      .total_fertility_rate = parse_data<real_type>(data, "tfr", proj_years),
      .PMTCT = parse_data<real_type>(data, "pmtct", SS::hPS, proj_years),
      .vertical_transmission_rate = parse_data<real_type>(data, "mtct", SS::hDS + 1, SS::hVT),
      .PMTCT_transmission_rate = parse_data<real_type>(data, "pmtct_mtct", SS::hDS, SS::hPS, SS::hVT),
      .PMTCT_dropout = parse_data<real_type>(data, "pmtct_dropout", SS::hPS_dropout, proj_years),
      .PMTCT_input_is_percent = parse_data<int>(data, "pmtct_input_isperc", proj_years),
      .breastfeeding_duration_art = parse_data<real_type>(data, "bf_duration_art", SS::hBF, proj_years),
      .breastfeeding_duration_no_art = parse_data<real_type>(data, "bf_duration_no_art", SS::hBF, proj_years),
      .mat_hiv_births = parse_data<real_type>(data, "mat_hiv_births", proj_years),
      .mat_prev_input = parse_data<int>(data, "mat_prev_input", proj_years),
      .prop_lt200 = parse_data<real_type>(data, "prop_lt200", proj_years),
      .prop_gte350 = parse_data<real_type>(data, "prop_gte350", proj_years),
      .incrate = parse_data<real_type>(data, "incrate", proj_years),
      .ctx_val_is_percent = parse_data<int>(data, "ctx_val_ispercent", proj_years),
      .hc_art_is_age_spec = parse_data<int>(data, "paed_art_age_spec", proj_years),
      .hc_age_coarse = parse_data<real_type>(data, "hc_age_coarse", SS::hcAG_end),
      .abortion = parse_data<real_type>(data, "abortion", SS::hAB_ind, proj_years),
      .patients_reallocated = parse_data<real_type>(data, "patients_reallocated", proj_years),
      .hc_art_ltfu = parse_data<real_type>(data, "paed_art_ltfu", proj_years),
      .hc_age_coarse_cd4 = parse_data<int>(data, "hc_age_coarse_cd4", opts.p_idx_hiv_first_adult),
      .adult_female_infections = parse_data<real_type>(data, "adult_female_infections", opts.p_fertility_age_groups, proj_years),
      .adult_female_hivnpop = parse_data<real_type>(data, "hivnpop", opts.p_fertility_age_groups, proj_years),
      .total_births = parse_data<real_type>(data, "total_births", proj_years),
      .ctx_effect = Rcpp::as<real_type>(data["ctx_effect"]),
      .local_adj_factor = Rcpp::as<real_type>(data["laf"])
    };
  };

  struct Intermediate {
    TFS<real_type, SS::hDS, SS::NS> age15_hiv_pop;
    TFS<real_type, SS::hTS, SS::hDS, SS::NS> age15_art_pop;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hAG, SS::NS> hc_posthivmort;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hAG, SS::NS> hc_grad;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hAG, SS::NS> hc_ctx_need;
    TFS<real_type, SS::hDS, SS::hAG, SS::NS> eligible;
    TFS<real_type, 4> unmet_need;
    TFS<real_type, 4> total_need;
    TFS<real_type, 4> on_art;
    TFS<real_type, 4> total_art_last_year;
    TFS<real_type, 4> total_art_this_year;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hAG, SS::NS> hc_art_grad;
    TFS<real_type, 4> hc_art_scalar;
    TFS<real_type, 4> hc_initByAge;
    TFS<real_type, 4> hc_adj;
    TFS<real_type, 4> hc_art_deaths;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hcAG_end, SS::NS> hc_hiv_dist;
    TFS<real_type, SS::hDS,  SS::hcAG_end, SS::NS> hc_hiv_total;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hcAG_end, SS::NS> art_ltfu_grad;
    TFS<real_type, SS::hPS> previous_mtct;
    TFS<real_type, SS::hAG, SS::NS> p_hiv_neg_pop;
    TFS<real_type, SS::hPS> PMTCT_coverage;
    TFS<real_type, SS::hBF_coarse> bf_transmission_rate;
    real_type retained;
    real_type hc_death_rate;
    real_type asfr_sum;
    real_type births_sum;
    real_type nHIVcurr;
    real_type nHIVlast;
    real_type df;
    real_type prev;
    real_type birthsCurrAge;
    real_type birthsHE;
    real_type births_HE_15_24;
    real_type sumARV;
    real_type need_PMTCT;
    real_type OnPMTCT;
    real_type num_wlhiv_lt200;
    real_type num_wlhiv_200to350;
    real_type num_wlhiv_gte350;
    real_type num_wlhiv;
    real_type prop_wlhiv_lt200;
    real_type prop_wlhiv_200to350;
    real_type prop_wlhiv_gte350;
    real_type prop_wlhiv_lt350;
    real_type excessratio;
    real_type excessratio_bf;
    real_type optA_transmission_rate;
    real_type optB_transmission_rate;
    real_type optA_bf_transmission_rate;
    real_type optB_bf_transmission_rate;
    real_type retained_on_ART;
    real_type retained_started_ART;
    real_type perinatal_transmission_rate;
    real_type receiving_PMTCT;
    real_type no_PMTCT;
    real_type perinatal_transmission_rate_bf_calc;
    real_type age_weighted_hivneg;
    real_type age_weighted_infections;
    real_type incidence_rate_wlhiv;
    real_type perinatal_transmission_from_incidence;
    real_type bf_at_risk;
    real_type bf_incident_hiv_transmission_rate;
    real_type percent_no_treatment;
    real_type percent_on_treatment;
    real_type ctx_coverage;
    real_type need_cotrim;

    Intermediate() {};

    void reset() {
      age15_hiv_pop.setZero();
      age15_art_pop.setZero();
      hc_posthivmort.setZero();
      hc_grad.setZero();
      hc_ctx_need.setZero();
      eligible.setZero();
      unmet_need.setZero();
      total_need.setZero();
      on_art.setZero();
      total_art_last_year.setZero();
      total_art_this_year.setZero();
      hc_art_grad.setZero();
      hc_art_scalar.setZero();
      hc_initByAge.setZero();
      hc_adj.setZero();
      hc_art_deaths.setZero();
      hc_hiv_dist.setZero();
      hc_hiv_total.setZero();
      art_ltfu_grad.setZero();
      previous_mtct.setZero();
      p_hiv_neg_pop.setZero();
      PMTCT_coverage.setZero();
      bf_transmission_rate.setZero();
      retained = 0;
      hc_death_rate = 0;
      asfr_sum = 0;
      births_sum = 0;
      nHIVcurr = 0;
      nHIVlast = 0;
      df = 0;
      prev = 0;
      birthsCurrAge = 0;
      birthsHE = 0;
      births_HE_15_24 = 0;
      sumARV = 0;
      need_PMTCT = 0;
      OnPMTCT = 0;
      num_wlhiv_lt200 = 0;
      num_wlhiv_200to350 = 0;
      num_wlhiv_gte350 = 0;
      num_wlhiv = 0;
      prop_wlhiv_lt200 = 0;
      prop_wlhiv_200to350 = 0;
      prop_wlhiv_gte350 = 0;
      prop_wlhiv_lt350 = 0;
      excessratio = 0;
      excessratio_bf = 0;
      optA_transmission_rate = 0;
      optB_transmission_rate = 0;
      optA_bf_transmission_rate = 0;
      optB_bf_transmission_rate = 0;
      retained_on_ART = 0;
      retained_started_ART = 0;
      perinatal_transmission_rate = 0;
      receiving_PMTCT = 0;
      no_PMTCT = 0;
      perinatal_transmission_rate_bf_calc = 0;
      age_weighted_hivneg = 0;
      age_weighted_infections = 0;
      incidence_rate_wlhiv = 0;
      perinatal_transmission_from_incidence = 0;
      bf_at_risk = 0;
      bf_incident_hiv_transmission_rate = 0;
      percent_no_treatment = 0;
      percent_on_treatment = 0;
      ctx_coverage = 0;
      need_cotrim = 0;
    };
  };

  struct State {
    TFS<real_type, SS::hc1DS, SS::hcTT, SS::hc1AG, SS::NS> hc1_hiv_pop;
    TFS<real_type, SS::hc2DS, SS::hcTT, SS::hc2AG, SS::NS> hc2_hiv_pop;
    TFS<real_type, SS::hTS, SS::hc1DS, SS::hc1AG, SS::NS> hc1_art_pop;
    TFS<real_type, SS::hTS, SS::hc2DS, SS::hc2AG, SS::NS> hc2_art_pop;
    TFS<real_type, SS::hc1DS, SS::hcTT, SS::hc1AG, SS::NS> hc1_noart_aids_deaths;
    TFS<real_type, SS::hc2DS, SS::hcTT, SS::hc2AG, SS::NS> hc2_noart_aids_deaths;
    TFS<real_type, SS::hTS, SS::hc1DS, SS::hc1AG, SS::NS> hc1_art_aids_deaths;
    TFS<real_type, SS::hTS, SS::hc2DS, SS::hc2AG, SS::NS> hc2_art_aids_deaths;
    TFS<real_type, SS::hcAG_coarse> hc_art_init;
    TFS<real_type, SS::hc1DS, SS::hcTT, SS::hcAG_end, SS::NS> hc_art_need_init;
    real_type hiv_births;
    real_type ctx_need;
    real_type ctx_mean;

    State() {
      reset();
    };

    State(const State& initial_state):
      hc1_hiv_pop(initial_state.hc1_hiv_pop),
      hc2_hiv_pop(initial_state.hc2_hiv_pop),
      hc1_art_pop(initial_state.hc1_art_pop),
      hc2_art_pop(initial_state.hc2_art_pop),
      hc1_noart_aids_deaths(initial_state.hc1_noart_aids_deaths),
      hc2_noart_aids_deaths(initial_state.hc2_noart_aids_deaths),
      hc1_art_aids_deaths(initial_state.hc1_art_aids_deaths),
      hc2_art_aids_deaths(initial_state.hc2_art_aids_deaths),
      hc_art_init(initial_state.hc_art_init),
      hc_art_need_init(initial_state.hc_art_need_init),
      hiv_births(initial_state.hiv_births),
      ctx_need(initial_state.ctx_need),
      ctx_mean(initial_state.ctx_mean)
    {};

    void reset() {
      hc1_hiv_pop.setZero();
      hc2_hiv_pop.setZero();
      hc1_art_pop.setZero();
      hc2_art_pop.setZero();
      hc1_noart_aids_deaths.setZero();
      hc2_noart_aids_deaths.setZero();
      hc1_art_aids_deaths.setZero();
      hc2_art_aids_deaths.setZero();
      hc_art_init.setZero();
      hc_art_need_init.setZero();
      hiv_births = 0;
      ctx_need = 0;
      ctx_mean = 0;
    };
  };

  struct OutputState {
    T5<real_type> hc1_hiv_pop;
    T5<real_type> hc2_hiv_pop;
    T5<real_type> hc1_art_pop;
    T5<real_type> hc2_art_pop;
    T5<real_type> hc1_noart_aids_deaths;
    T5<real_type> hc2_noart_aids_deaths;
    T5<real_type> hc1_art_aids_deaths;
    T5<real_type> hc2_art_aids_deaths;
    T2<real_type> hc_art_init;
    T5<real_type> hc_art_need_init;
    T1<real_type> hiv_births;
    T1<real_type> ctx_need;
    T1<real_type> ctx_mean;

    OutputState(int output_years):
      hc1_hiv_pop(SS::hc1DS, SS::hcTT, SS::hc1AG, SS::NS, output_years),
      hc2_hiv_pop(SS::hc2DS, SS::hcTT, SS::hc2AG, SS::NS, output_years),
      hc1_art_pop(SS::hTS, SS::hc1DS, SS::hc1AG, SS::NS, output_years),
      hc2_art_pop(SS::hTS, SS::hc2DS, SS::hc2AG, SS::NS, output_years),
      hc1_noart_aids_deaths(SS::hc1DS, SS::hcTT, SS::hc1AG, SS::NS, output_years),
      hc2_noart_aids_deaths(SS::hc2DS, SS::hcTT, SS::hc2AG, SS::NS, output_years),
      hc1_art_aids_deaths(SS::hTS, SS::hc1DS, SS::hc1AG, SS::NS, output_years),
      hc2_art_aids_deaths(SS::hTS, SS::hc2DS, SS::hc2AG, SS::NS, output_years),
      hc_art_init(SS::hcAG_coarse, output_years),
      hc_art_need_init(SS::hc1DS, SS::hcTT, SS::hcAG_end, SS::NS, output_years),
      hiv_births(output_years),
      ctx_need(output_years),
      ctx_mean(output_years)
    {
      hc1_hiv_pop.setZero();
      hc2_hiv_pop.setZero();
      hc1_art_pop.setZero();
      hc2_art_pop.setZero();
      hc1_noart_aids_deaths.setZero();
      hc2_noart_aids_deaths.setZero();
      hc1_art_aids_deaths.setZero();
      hc2_art_aids_deaths.setZero();
      hc_art_init.setZero();
      hc_art_need_init.setZero();
      hiv_births.setZero();
      ctx_need.setZero();
      ctx_mean.setZero();
    };

    void save_state(const size_t i, const State &state) {
      hc1_hiv_pop.chip(i, hc1_hiv_pop.NumDimensions - 1) = state.hc1_hiv_pop;
      hc2_hiv_pop.chip(i, hc2_hiv_pop.NumDimensions - 1) = state.hc2_hiv_pop;
      hc1_art_pop.chip(i, hc1_art_pop.NumDimensions - 1) = state.hc1_art_pop;
      hc2_art_pop.chip(i, hc2_art_pop.NumDimensions - 1) = state.hc2_art_pop;
      hc1_noart_aids_deaths.chip(i, hc1_noart_aids_deaths.NumDimensions - 1) = state.hc1_noart_aids_deaths;
      hc2_noart_aids_deaths.chip(i, hc2_noart_aids_deaths.NumDimensions - 1) = state.hc2_noart_aids_deaths;
      hc1_art_aids_deaths.chip(i, hc1_art_aids_deaths.NumDimensions - 1) = state.hc1_art_aids_deaths;
      hc2_art_aids_deaths.chip(i, hc2_art_aids_deaths.NumDimensions - 1) = state.hc2_art_aids_deaths;
      hc_art_init.chip(i, hc_art_init.NumDimensions - 1) = state.hc_art_init;
      hc_art_need_init.chip(i, hc_art_need_init.NumDimensions - 1) = state.hc_art_need_init;
      hiv_births(i) = state.hiv_births;
      ctx_need(i) = state.ctx_need;
      ctx_mean(i) = state.ctx_mean;
    };
  };

  static constexpr int output_count = 13;
  static int get_build_output_size(int prev_size) {
    return prev_size + output_count;
  };

  static int build_output(
    Rcpp::List& ret,
    Rcpp::CharacterVector& names,
    int index,
    const OutputState& state,
    const size_t& output_years
  ) {
    Rcpp::NumericVector r_hc1_hiv_pop(SS::hc1DS * SS::hcTT * SS::hc1AG * SS::NS * output_years);
    r_hc1_hiv_pop.attr("dim") = Rcpp::IntegerVector::create(SS::hc1DS, SS::hcTT, SS::hc1AG, SS::NS, output_years);
    std::copy_n(state.hc1_hiv_pop.data(), state.hc1_hiv_pop.size(), REAL(r_hc1_hiv_pop));
    names[index + 0] = "hc1_hiv_pop";
    ret[index + 0] = r_hc1_hiv_pop;
    Rcpp::NumericVector r_hc2_hiv_pop(SS::hc2DS * SS::hcTT * SS::hc2AG * SS::NS * output_years);
    r_hc2_hiv_pop.attr("dim") = Rcpp::IntegerVector::create(SS::hc2DS, SS::hcTT, SS::hc2AG, SS::NS, output_years);
    std::copy_n(state.hc2_hiv_pop.data(), state.hc2_hiv_pop.size(), REAL(r_hc2_hiv_pop));
    names[index + 1] = "hc2_hiv_pop";
    ret[index + 1] = r_hc2_hiv_pop;
    Rcpp::NumericVector r_hc1_art_pop(SS::hTS * SS::hc1DS * SS::hc1AG * SS::NS * output_years);
    r_hc1_art_pop.attr("dim") = Rcpp::IntegerVector::create(SS::hTS, SS::hc1DS, SS::hc1AG, SS::NS, output_years);
    std::copy_n(state.hc1_art_pop.data(), state.hc1_art_pop.size(), REAL(r_hc1_art_pop));
    names[index + 2] = "hc1_art_pop";
    ret[index + 2] = r_hc1_art_pop;
    Rcpp::NumericVector r_hc2_art_pop(SS::hTS * SS::hc2DS * SS::hc2AG * SS::NS * output_years);
    r_hc2_art_pop.attr("dim") = Rcpp::IntegerVector::create(SS::hTS, SS::hc2DS, SS::hc2AG, SS::NS, output_years);
    std::copy_n(state.hc2_art_pop.data(), state.hc2_art_pop.size(), REAL(r_hc2_art_pop));
    names[index + 3] = "hc2_art_pop";
    ret[index + 3] = r_hc2_art_pop;
    Rcpp::NumericVector r_hc1_noart_aids_deaths(SS::hc1DS * SS::hcTT * SS::hc1AG * SS::NS * output_years);
    r_hc1_noart_aids_deaths.attr("dim") = Rcpp::IntegerVector::create(SS::hc1DS, SS::hcTT, SS::hc1AG, SS::NS, output_years);
    std::copy_n(state.hc1_noart_aids_deaths.data(), state.hc1_noart_aids_deaths.size(), REAL(r_hc1_noart_aids_deaths));
    names[index + 4] = "hc1_noart_aids_deaths";
    ret[index + 4] = r_hc1_noart_aids_deaths;
    Rcpp::NumericVector r_hc2_noart_aids_deaths(SS::hc2DS * SS::hcTT * SS::hc2AG * SS::NS * output_years);
    r_hc2_noart_aids_deaths.attr("dim") = Rcpp::IntegerVector::create(SS::hc2DS, SS::hcTT, SS::hc2AG, SS::NS, output_years);
    std::copy_n(state.hc2_noart_aids_deaths.data(), state.hc2_noart_aids_deaths.size(), REAL(r_hc2_noart_aids_deaths));
    names[index + 5] = "hc2_noart_aids_deaths";
    ret[index + 5] = r_hc2_noart_aids_deaths;
    Rcpp::NumericVector r_hc1_art_aids_deaths(SS::hTS * SS::hc1DS * SS::hc1AG * SS::NS * output_years);
    r_hc1_art_aids_deaths.attr("dim") = Rcpp::IntegerVector::create(SS::hTS, SS::hc1DS, SS::hc1AG, SS::NS, output_years);
    std::copy_n(state.hc1_art_aids_deaths.data(), state.hc1_art_aids_deaths.size(), REAL(r_hc1_art_aids_deaths));
    names[index + 6] = "hc1_art_aids_deaths";
    ret[index + 6] = r_hc1_art_aids_deaths;
    Rcpp::NumericVector r_hc2_art_aids_deaths(SS::hTS * SS::hc2DS * SS::hc2AG * SS::NS * output_years);
    r_hc2_art_aids_deaths.attr("dim") = Rcpp::IntegerVector::create(SS::hTS, SS::hc2DS, SS::hc2AG, SS::NS, output_years);
    std::copy_n(state.hc2_art_aids_deaths.data(), state.hc2_art_aids_deaths.size(), REAL(r_hc2_art_aids_deaths));
    names[index + 7] = "hc2_art_aids_deaths";
    ret[index + 7] = r_hc2_art_aids_deaths;
    Rcpp::NumericVector r_hc_art_init(SS::hcAG_coarse * output_years);
    r_hc_art_init.attr("dim") = Rcpp::IntegerVector::create(SS::hcAG_coarse, output_years);
    std::copy_n(state.hc_art_init.data(), state.hc_art_init.size(), REAL(r_hc_art_init));
    names[index + 8] = "hc_art_init";
    ret[index + 8] = r_hc_art_init;
    Rcpp::NumericVector r_hc_art_need_init(SS::hc1DS * SS::hcTT * SS::hcAG_end * SS::NS * output_years);
    r_hc_art_need_init.attr("dim") = Rcpp::IntegerVector::create(SS::hc1DS, SS::hcTT, SS::hcAG_end, SS::NS, output_years);
    std::copy_n(state.hc_art_need_init.data(), state.hc_art_need_init.size(), REAL(r_hc_art_need_init));
    names[index + 9] = "hc_art_need_init";
    ret[index + 9] = r_hc_art_need_init;
    Rcpp::NumericVector r_hiv_births(output_years);
    r_hiv_births.attr("dim") = Rcpp::IntegerVector::create(output_years);
    std::copy_n(state.hiv_births.data(), state.hiv_births.size(), REAL(r_hiv_births));
    names[index + 10] = "hiv_births";
    ret[index + 10] = r_hiv_births;
    Rcpp::NumericVector r_ctx_need(output_years);
    r_ctx_need.attr("dim") = Rcpp::IntegerVector::create(output_years);
    std::copy_n(state.ctx_need.data(), state.ctx_need.size(), REAL(r_ctx_need));
    names[index + 11] = "ctx_need";
    ret[index + 11] = r_ctx_need;
    Rcpp::NumericVector r_ctx_mean(output_years);
    r_ctx_mean.attr("dim") = Rcpp::IntegerVector::create(output_years);
    std::copy_n(state.ctx_mean.data(), state.ctx_mean.size(), REAL(r_ctx_mean));
    names[index + 12] = "ctx_mean";
    ret[index + 12] = r_ctx_mean;
    return index + output_count;
  };
};

}
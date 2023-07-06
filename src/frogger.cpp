#include <Rcpp.h>

#include "frogger.hpp"
#include "types.hpp"
#include "state_space.hpp"

int transform_simulation_years(const Rcpp::List demp, SEXP r_sim_years) {
  Rcpp::NumericVector Sx = demp["Sx"];
  Rcpp::Dimension d = Sx.attr("dim");
  // Simulation initialises state from first years input data (index 0)
  // then runs for each year simulating this years (i) data using previous years
  // state (i - 1) and this years input data (i). So -1 off index for max years
  // to simulate as index 0 used for initial state
  const int max_sim_years = d[2] - 1;
  if (r_sim_years == R_NilValue) {
    return max_sim_years;
  }
  auto sim_years = INTEGER(r_sim_years)[0];
  if (sim_years > max_sim_years) {
    Rcpp::stop("No of years > max years of " + std::to_string(max_sim_years));
  }
  return sim_years;
}

int transform_hiv_steps_per_year(SEXP r_hiv_steps_per_year) {
  int hiv_steps_per_year;
  if (r_hiv_steps_per_year == R_NilValue) {
    hiv_steps_per_year = 10;
  } else {
    hiv_steps_per_year = INTEGER(r_hiv_steps_per_year)[0];
  }
  return hiv_steps_per_year;
}

template <leapfrog::HivAgeStratification S>
leapfrog::StateSpace<S> get_state_space(const std::string hiv_age_stratification) {
  leapfrog::StateSpace<S> ss;
  if (hiv_age_stratification == "full") {
    ss = leapfrog::StateSpace<leapfrog::HivAgeStratification::full>();
  } else {
    // We've already validated that hiv_age_stratification is one of "full" or "coarse"
    ss = leapfrog::StateSpace<leapfrog::HivAgeStratification::coarse>();
  }
  return ss;
}

template <typename T>
T* r_data(SEXP x) {
  static_assert(sizeof(T) == 0, "Only specializations of r_data can be used");
}

template <>
double* r_data(SEXP x) {
  return REAL(x);
}

template <>
int * r_data(SEXP x) {
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

std::vector<int> transform_output_steps(Rcpp::NumericVector output_steps) {
  return Rcpp::as<std::vector<int>>(output_steps);
}

template <leapfrog::HivAgeStratification S>
Rcpp::List fit_model(const leapfrog::StateSpace<S> ss,
                     const Rcpp::List data,
                     const Rcpp::List projection_parameters,
                     const int proj_years,
                     const int hiv_steps,
                     const std::vector<int> save_steps) {
  const double dt = (1.0 / hiv_steps);
  const int fertility_first_age_group = 15;
  const int age_groups_fert = 35;
  const int hiv_adult_first_age_group = 15;
  const int adult_incidence_first_age_group = hiv_adult_first_age_group;
  // Hardcoded 15-49 for now (35 groups within this band)
  const int pAG_INCIDPOP = 35;
  // 0-based indexing vs R 1-based
  const int time_art_start =
      Rcpp::as<int>(projection_parameters["t_ART_start"]) - 1;
  int age_groups_hiv_15plus = ss.age_groups_hiv;
  const int scale_cd4_mortality =
      Rcpp::as<int>(projection_parameters["scale_cd4_mort"]);
  int hIDX_15PLUS = 0;
  const double art_alloc_mxweight = Rcpp::as<double>(projection_parameters["art_alloc_mxweight"]);

  const leapfrog::TensorMap2<double> base_pop = parse_data<double>(data, "basepop",
                                                                   ss.age_groups_pop, ss.num_genders);

  // Survival has size age_groups_pop + 1 as this is the probability of
  // surviving between ages, so from 0 to 1, 1 to 2, ..., 79 to 80+ and
  // 80+ to 80+
  const leapfrog::TensorMap3<double> survival = parse_data<double>(data, "Sx",
                                                                   ss.age_groups_pop + 1, ss.num_genders, proj_years);
  const leapfrog::TensorMap3<double> net_migration = parse_data<double>(data, "netmigr_adj",
                                                                        ss.age_groups_pop, ss.num_genders, proj_years);
  const leapfrog::TensorMap2<double> age_sex_fertility_ratio = parse_data<double>(data, "asfr",
                                                                                  age_groups_fert, proj_years);
  const leapfrog::TensorMap2<double> births_sex_prop = parse_data<double>(data, "births_sex_prop",
                                                                          ss.num_genders, proj_years);
  const leapfrog::TensorMap1<double> incidence_rate = parse_data<double>(projection_parameters, "incidinput",
                                                                         proj_years);
  const leapfrog::TensorMap3<double> incidence_relative_risk_age = parse_data<double>(
      projection_parameters, "incrr_age", ss.age_groups_pop - hiv_adult_first_age_group, ss.num_genders, proj_years);
  const leapfrog::TensorMap1<double> incidence_relative_risk_sex = parse_data<double>(projection_parameters,
                                                                                      "incrr_sex", proj_years);
  const leapfrog::TensorMap3<double> cd4_mortality = parse_data<double>(projection_parameters, "cd4_mort",
                                                                        ss.disease_stages, ss.age_groups_hiv, ss.num_genders);
  const leapfrog::TensorMap3<double> cd4_progression = parse_data<double>(projection_parameters, "cd4_prog",
                                                                          ss.disease_stages - 1, ss.age_groups_hiv,
                                                                          ss.num_genders);

  leapfrog::Tensor1<int> artcd4elig_idx = parse_data<int>(projection_parameters, "artcd4elig_idx", proj_years + 1);
  for (int i = 0; i <= proj_years; ++i) {
    // 0-based indexing in C++ vs 1-based indexing in R
    artcd4elig_idx(i) = artcd4elig_idx(i) - 1;
  }

  const leapfrog::TensorMap3<double> cd4_initdist = parse_data<double>(projection_parameters, "cd4_initdist",
                                                                       ss.disease_stages, ss.age_groups_hiv, ss.num_genders);
  const leapfrog::TensorMap4<double> art_mortality = parse_data<double>(projection_parameters, "art_mort",
                                                                        ss.treatment_stages, ss.disease_stages,
                                                                        ss.age_groups_hiv, ss.num_genders);
  const leapfrog::TensorMap2<double> artmx_timerr = parse_data<double>(projection_parameters, "artmx_timerr",
                                                                       ss.treatment_stages, proj_years);
  leapfrog::Tensor1<double> h_art_stage_dur(ss.treatment_stages - 1);
  h_art_stage_dur.setConstant(0.5);

  const leapfrog::TensorMap1<double> art_dropout = parse_data<double>(projection_parameters, "art_dropout", proj_years);
  const leapfrog::TensorMap2<double> art15plus_num = parse_data<double>(projection_parameters, "art15plus_num",
                                                                        ss.num_genders, proj_years);
  const leapfrog::TensorMap2<int> art15plus_isperc = parse_data<int>(projection_parameters, "art15plus_isperc",
                                                                     ss.num_genders, proj_years);

  const leapfrog::Parameters<double> params = {fertility_first_age_group,
                                               age_groups_fert,
                                               age_groups_hiv_15plus,
                                               hiv_adult_first_age_group,
                                               time_art_start,
                                               adult_incidence_first_age_group,
                                               pAG_INCIDPOP,
                                               hiv_steps,
                                               dt,
                                               scale_cd4_mortality,
                                               hIDX_15PLUS,
                                               art_alloc_mxweight,
                                               incidence_rate,
                                               base_pop,
                                               survival,
                                               net_migration,
                                               age_sex_fertility_ratio,
                                               births_sex_prop,
                                               incidence_relative_risk_age,
                                               incidence_relative_risk_sex,
                                               cd4_mortality,
                                               cd4_progression,
                                               artcd4elig_idx,
                                               cd4_initdist,
                                               art_mortality,
                                               artmx_timerr,
                                               h_art_stage_dur,
                                               art_dropout,
                                               art15plus_num,
                                               art15plus_isperc};

  auto state = leapfrog::run_model<S, double>(proj_years, save_steps, params);

  size_t output_years = save_steps.size();
  Rcpp::NumericVector r_total_population(ss.age_groups_pop * ss.num_genders * output_years);
  Rcpp::NumericVector r_births(output_years);
  Rcpp::NumericVector r_natural_deaths(ss.age_groups_pop * ss.num_genders * output_years);
  Rcpp::NumericVector r_hiv_population(ss.age_groups_pop * ss.num_genders * output_years);
  Rcpp::NumericVector r_hiv_natural_deaths(ss.age_groups_pop * ss.num_genders * output_years);
  Rcpp::NumericVector r_hiv_strat_adult(ss.disease_stages * ss.age_groups_hiv *
                                        ss.num_genders * output_years);
  Rcpp::NumericVector r_art_strat_adult(ss.treatment_stages * ss.disease_stages *
                                        ss.age_groups_hiv * ss.num_genders * output_years);
  Rcpp::NumericVector r_aids_deaths_no_art(ss.disease_stages * ss.age_groups_hiv * ss.num_genders * output_years);
  Rcpp::NumericVector r_infections(ss.age_groups_pop * ss.num_genders * output_years);
  Rcpp::NumericVector r_aids_deaths_art(ss.treatment_stages * ss.disease_stages * ss.age_groups_hiv *
                                        ss.num_genders * output_years);
  Rcpp::NumericVector r_art_initiation(ss.disease_stages * ss.age_groups_hiv * ss.num_genders * output_years);
  Rcpp::NumericVector r_hiv_deaths(ss.age_groups_pop * ss.num_genders * output_years);

  r_total_population.attr("dim") =
      Rcpp::NumericVector::create(ss.age_groups_pop, ss.num_genders, output_years);
  r_natural_deaths.attr("dim") =
      Rcpp::NumericVector::create(ss.age_groups_pop, ss.num_genders, output_years);
  r_hiv_population.attr("dim") =
      Rcpp::NumericVector::create(ss.age_groups_pop, ss.num_genders, output_years);
  r_hiv_natural_deaths.attr("dim") =
      Rcpp::NumericVector::create(ss.age_groups_pop, ss.num_genders, output_years);
  r_hiv_strat_adult.attr("dim") =
      Rcpp::NumericVector::create(ss.disease_stages, ss.age_groups_hiv, ss.num_genders, output_years);
  r_art_strat_adult.attr("dim") = Rcpp::NumericVector::create(
      ss.treatment_stages, ss.disease_stages, ss.age_groups_hiv, ss.num_genders, output_years);
  r_births.attr("dim") = Rcpp::NumericVector::create(output_years);
  r_aids_deaths_no_art.attr("dim") = Rcpp::NumericVector::create(
      ss.disease_stages, ss.age_groups_hiv, ss.num_genders, output_years);
  r_infections.attr("dim") = Rcpp::NumericVector::create(ss.age_groups_pop, ss.num_genders, output_years);
  r_aids_deaths_art.attr("dim") = Rcpp::NumericVector::create(
      ss.treatment_stages, ss.disease_stages, ss.age_groups_hiv, ss.num_genders, output_years);
  r_art_initiation.attr("dim") = Rcpp::NumericVector::create(
      ss.disease_stages, ss.age_groups_hiv, ss.num_genders, output_years);
  r_hiv_deaths.attr("dim") = Rcpp::NumericVector::create(ss.age_groups_pop, ss.num_genders, output_years);

  std::copy_n(state.total_population.data(), state.total_population.size(),
              REAL(r_total_population));
  std::copy_n(state.natural_deaths.data(), state.natural_deaths.size(),
              REAL(r_natural_deaths));
  std::copy_n(state.hiv_population.data(), state.hiv_population.size(),
              REAL(r_hiv_population));
  std::copy_n(state.hiv_natural_deaths.data(), state.hiv_natural_deaths.size(),
              REAL(r_hiv_natural_deaths));
  std::copy_n(state.hiv_strat_adult.data(), state.hiv_strat_adult.size(),
              REAL(r_hiv_strat_adult));
  std::copy_n(state.art_strat_adult.data(), state.art_strat_adult.size(),
              REAL(r_art_strat_adult));
  std::copy_n(state.births.data(), state.births.size(),
              REAL(r_births));
  std::copy_n(state.aids_deaths_no_art.data(), state.aids_deaths_no_art.size(),
              REAL(r_aids_deaths_no_art));
  std::copy_n(state.infections.data(), state.infections.size(),
              REAL(r_infections));
  std::copy_n(state.aids_deaths_art.data(), state.aids_deaths_art.size(),
              REAL(r_aids_deaths_art));
  std::copy_n(state.art_initiation.data(), state.art_initiation.size(),
              REAL(r_art_initiation));
  std::copy_n(state.hiv_deaths.data(), state.hiv_deaths.size(),
              REAL(r_hiv_deaths));

  Rcpp::List ret =
      Rcpp::List::create(Rcpp::_["total_population"] = r_total_population,
                         Rcpp::_["births"] = r_births,
                         Rcpp::_["natural_deaths"] = r_natural_deaths,
                         Rcpp::_["hiv_population"] = r_hiv_population,
                         Rcpp::_["hiv_natural_deaths"] = r_hiv_natural_deaths,
                         Rcpp::_["hiv_strat_adult"] = r_hiv_strat_adult,
                         Rcpp::_["art_strat_adult"] = r_art_strat_adult,
                         Rcpp::_["aids_deaths_no_art"] = r_aids_deaths_no_art,
                         Rcpp::_["infections"] = r_infections,
                         Rcpp::_["aids_deaths_art"] = r_aids_deaths_art,
                         Rcpp::_["art_initiation"] = r_art_initiation,
                         Rcpp::_["hiv_deaths"] = r_hiv_deaths);
  return ret;
}

// [[Rcpp::export]]
Rcpp::List run_base_model(const Rcpp::List data,
                          const Rcpp::List projection_parameters,
                          SEXP sim_years,
                          SEXP hiv_steps_per_year,
                          Rcpp::NumericVector output_steps,
                          std::string hiv_age_stratification) {
  const int proj_years = transform_simulation_years(data, sim_years);
  const std::vector<int> save_steps = transform_output_steps(output_steps);
  const int hiv_steps = transform_hiv_steps_per_year(hiv_steps_per_year);


  Rcpp::List ret;
  if (hiv_age_stratification == "full") {
    constexpr auto ss = leapfrog::StateSpace<leapfrog::HivAgeStratification::full>();
    ret = fit_model<leapfrog::HivAgeStratification::full>(ss, data, projection_parameters,
                                                          proj_years, hiv_steps, save_steps);
  } else {
    // We've already validated that hiv_age_stratification is one of "full" or "coarse"
    constexpr auto ss = leapfrog::StateSpace<leapfrog::HivAgeStratification::coarse>();
    ret = fit_model<leapfrog::HivAgeStratification::coarse>(ss, data, projection_parameters,
                                                            proj_years, hiv_steps, save_steps);
  }

  return ret;
}

#pragma once

#include <Rcpp.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "types.hpp"

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

template<typename real_type, leapfrog::HivAgeStratification S>
leapfrog::Parameters <real_type> setup_model_params(const Rcpp::List data,
                                                    const leapfrog::Options<double> options,
                                                    const int proj_years) {
  constexpr auto ss = leapfrog::StateSpace<S>();
  const leapfrog::TensorMap2<double> base_pop = parse_data<double>(data, "basepop",
                                                                   ss.age_groups_pop, ss.num_genders);
  const leapfrog::TensorMap3<double> survival = parse_data<double>(data, "Sx",
                                                                   ss.age_groups_pop + 1, ss.num_genders, proj_years);
  const leapfrog::TensorMap3<double> net_migration = parse_data<double>(data, "netmigr_adj",
                                                                        ss.age_groups_pop, ss.num_genders, proj_years);
  const leapfrog::TensorMap2<double> age_sex_fertility_ratio = parse_data<double>(data, "asfr",
                                                                                  options.age_groups_fert, proj_years);
  const leapfrog::TensorMap2<double> births_sex_prop = parse_data<double>(data, "births_sex_prop",
                                                                          ss.num_genders, proj_years);
  const leapfrog::TensorMap1<double> incidence_rate = parse_data<double>(data, "incidinput",
                                                                         proj_years);
  const leapfrog::TensorMap3<double> incidence_relative_risk_age = parse_data<double>(
      data, "incrr_age", ss.age_groups_pop - options.hiv_adult_first_age_group, ss.num_genders, proj_years);
  const leapfrog::TensorMap1<double> incidence_relative_risk_sex = parse_data<double>(data,
                                                                                      "incrr_sex", proj_years);
  const leapfrog::TensorMap3<double> cd4_mortality = parse_data<double>(data, "cd4_mort",
                                                                        ss.disease_stages, ss.age_groups_hiv,
                                                                        ss.num_genders);
  const leapfrog::TensorMap3<double> cd4_progression = parse_data<double>(data, "cd4_prog",
                                                                          ss.disease_stages - 1, ss.age_groups_hiv,
                                                                          ss.num_genders);

  leapfrog::Tensor1<int> artcd4elig_idx = parse_data<int>(data, "artcd4elig_idx", proj_years + 1);
  for (int i = 0; i <= proj_years; ++i) {
    // 0-based indexing in C++ vs 1-based indexing in R
    artcd4elig_idx(i) = artcd4elig_idx(i) - 1;
  }

  const leapfrog::TensorMap3<double> cd4_initdist = parse_data<double>(data, "cd4_initdist",
                                                                       ss.disease_stages, ss.age_groups_hiv,
                                                                       ss.num_genders);
  const leapfrog::TensorMap4<double> art_mortality = parse_data<double>(data, "art_mort",
                                                                        ss.treatment_stages, ss.disease_stages,
                                                                        ss.age_groups_hiv, ss.num_genders);
  const leapfrog::TensorMap2<double> artmx_timerr = parse_data<double>(data, "artmx_timerr",
                                                                       ss.treatment_stages, proj_years);
  leapfrog::Tensor1<double> h_art_stage_dur(ss.treatment_stages - 1);
  h_art_stage_dur.setConstant(0.5);

  const leapfrog::TensorMap1<double> art_dropout = parse_data<double>(data, "art_dropout", proj_years);
  const leapfrog::TensorMap2<double> art15plus_num = parse_data<double>(data, "art15plus_num",
                                                                        ss.num_genders, proj_years);
  const leapfrog::TensorMap2<int> art15plus_isperc = parse_data<int>(data, "art15plus_isperc",
                                                                     ss.num_genders, proj_years);

  const leapfrog::TensorMap1<double> hc_nosocomial = parse_data<double>(data, "paed_incid_input", proj_years);
  const leapfrog::TensorMap1<double> hc1_cd4_dist = parse_data<double>(data, "paed_cd4_dist", ss.hc1_disease_stages);
  const leapfrog::TensorMap2<double> hc_cd4_transition = parse_data<double>(data, "paed_cd4_transition", ss.hc2_disease_stages, ss.hc1_disease_stages);

  //natural history parameters
  const leapfrog::TensorMap3<double> hc1_cd4_mort = parse_data<double>(data, "paed_cd4_mort", ss.hc1_disease_stages, ss.hTM, ss.hc1_age_groups);
  const leapfrog::TensorMap3<double> hc2_cd4_mort = parse_data<double>(data, "adol_cd4_mort", ss.hc2_disease_stages, ss.hTM, ss.hc2_age_groups);

  const leapfrog::TensorMap1<double> hc1_cd4_prog = parse_data<double>(data, "paed_cd4_prog", ss.hc1_disease_stages);
  const leapfrog::TensorMap1<double> hc2_cd4_prog = parse_data<double>(data, "adol_cd4_prog", ss.hc2_disease_stages);

  // !!! TODO: Ask Rob about preferred approach to assigning a single value
  const double ctx_effect = parse_data<double>(data, "ctx_effect", 1)(0);  // select the first element of a TensorMap1 and assign to double
  const leapfrog::TensorMap1<double> ctx_val = parse_data<double>(data, "ctx_val", proj_years);

  // !!! TODO: fix hard coded values
  const leapfrog::TensorMap1<double> hc_art_elig_age = parse_data<double>(data, "paed_art_elig_age", proj_years);
  const leapfrog::TensorMap2<double> hc_art_elig_cd4 = parse_data<double>(data, "paed_art_elig_cd4", 15, proj_years);
  const leapfrog::TensorMap3<double> hc_art_mort_rr = parse_data<double>(data, "mort_art_rr", ss.treatment_stages, 15, proj_years);
  const leapfrog::TensorMap3<double> hc1_art_mort = parse_data<double>(data, "paed_art_mort", ss.hc1_disease_stages, ss.treatment_stages, ss.hc1_age_groups);
  const leapfrog::TensorMap3<double> hc2_art_mort = parse_data<double>(data, "adol_art_mort", ss.hc2_disease_stages, ss.treatment_stages, ss.hc2_age_groups);
  //!!! TODO: this needs to be a boolean
  const leapfrog::TensorMap1<int> hc_art_isperc = parse_data<int>(data, "artpaeds_isperc", proj_years);
  const leapfrog::TensorMap1<double> hc_art_val = parse_data<double>(data, "paed_art_val", proj_years);
  const leapfrog::TensorMap2<double> hc_art_init_dist = parse_data<double>(data, "init_art_dist", 15, proj_years);




  const leapfrog::Demography<double> demography = {
      base_pop,
      survival,
      net_migration,
      age_sex_fertility_ratio,
      births_sex_prop
  };

  const leapfrog::Incidence<double> incidence = {
      incidence_rate,
      incidence_relative_risk_age,
      incidence_relative_risk_sex
  };

  const leapfrog::NaturalHistory<double> natural_history = {
      cd4_mortality,
      cd4_progression,
      cd4_initdist
  };

  const leapfrog::Art<double> art = {
      artcd4elig_idx,
      art_mortality,
      artmx_timerr,
      h_art_stage_dur,
      art_dropout,
      art15plus_num,
      art15plus_isperc
  };

  const leapfrog::Children<double> children = {
      hc_nosocomial,
      hc1_cd4_dist,
      hc_cd4_transition,
      hc1_cd4_mort,
      hc2_cd4_mort,
      hc1_cd4_prog,
      hc2_cd4_prog,
      ctx_effect,
      ctx_val,
      hc_art_elig_age,
      hc_art_elig_cd4,
      hc_art_mort_rr,
      hc1_art_mort,
      hc2_art_mort,
      hc_art_isperc,
      hc_art_val,
      hc_art_init_dist

  };

  const leapfrog::Parameters<double> params = {options,
                                               demography,
                                               incidence,
                                               natural_history,
                                               art,
                                               children};
  return params;
}
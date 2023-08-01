// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `model_input.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include <Rcpp.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "types.hpp"
#include "r_utils.hpp"

template<leapfrog::HivAgeStratification S, typename real_type>
leapfrog::Parameters <real_type> setup_model_params(const Rcpp::List &data,
                                                    const leapfrog::Options<real_type> &options,
                                                    const int proj_years) {
  const int age_groups_fert = options.age_groups_fert;
  const int hiv_adult_first_age_group = options.hiv_adult_first_age_group;
  constexpr auto ss = leapfrog::StateSpace<S>();
  constexpr int num_genders = ss.num_genders;
  constexpr int age_groups_pop = ss.age_groups_pop;
  constexpr int age_groups_hiv = ss.age_groups_hiv;
  constexpr int disease_stages = ss.disease_stages;
  constexpr int treatment_stages = ss.treatment_stages;
  constexpr int hC1_disease_stages = ss.hC1_disease_stages;
  constexpr int hC2_disease_stages = ss.hC2_disease_stages;
  constexpr int hTM = ss.hTM;

  const leapfrog::TensorMap2<real_type> base_pop = parse_data<real_type>(data, "basepop", age_groups_pop, num_genders);
  const leapfrog::TensorMap3<real_type> survival = parse_data<real_type>(data, "Sx", age_groups_pop + 1, num_genders, proj_years);
  const leapfrog::TensorMap3<real_type> net_migration = parse_data<real_type>(data, "netmigr_adj", age_groups_pop, num_genders, proj_years);
  const leapfrog::TensorMap2<real_type> age_sex_fertility_ratio = parse_data<real_type>(data, "asfr", age_groups_fert, proj_years);
  const leapfrog::TensorMap2<real_type> births_sex_prop = parse_data<real_type>(data, "births_sex_prop", num_genders, proj_years);
  const leapfrog::TensorMap1<real_type> incidence_rate = parse_data<real_type>(data, "incidinput", proj_years);
  const leapfrog::TensorMap3<real_type> incidence_relative_risk_age = parse_data<real_type>(data, "incrr_age", age_groups_pop - hiv_adult_first_age_group, num_genders, proj_years);
  const leapfrog::TensorMap1<real_type> incidence_relative_risk_sex = parse_data<real_type>(data, "incrr_sex", proj_years);
  const leapfrog::TensorMap3<real_type> cd4_mortality = parse_data<real_type>(data, "cd4_mort", disease_stages, age_groups_hiv, num_genders);
  const leapfrog::TensorMap3<real_type> cd4_progression = parse_data<real_type>(data, "cd4_prog", disease_stages - 1, age_groups_hiv, num_genders);
  const leapfrog::TensorMap1<int> artcd4elig_idx = convert_base<1>(parse_data<int>(data, "artcd4elig_idx", proj_years + 1));
  const leapfrog::TensorMap3<real_type> cd4_initdist = parse_data<real_type>(data, "cd4_initdist", disease_stages, age_groups_hiv, num_genders);
  const leapfrog::TensorMap4<real_type> art_mortality = parse_data<real_type>(data, "art_mort", treatment_stages, disease_stages, age_groups_hiv, num_genders);
  const leapfrog::TensorMap2<real_type> artmx_timerr = parse_data<real_type>(data, "artmx_timerr", treatment_stages, proj_years);
  const leapfrog::TensorMap1<real_type> art_dropout = parse_data<real_type>(data, "art_dropout", proj_years);
  const leapfrog::TensorMap2<real_type> art15plus_num = parse_data<real_type>(data, "art15plus_num", num_genders, proj_years);
  const leapfrog::TensorMap2<int> art15plus_isperc = parse_data<int>(data, "art15plus_isperc", num_genders, proj_years);
  const leapfrog::TensorMap1<real_type> hc_nosocomial = parse_data<real_type>(data, "paed_incid_input", proj_years);
  const leapfrog::TensorMap1<real_type> hc1_cd4_dist = parse_data<real_type>(data, "paed_cd4_dist", hc2_disease_stages);
  const leapfrog::TensorMap2<real_type> hc_cd4_transition = parse_data<real_type>(data, "paed_cd4_transition", hc1_disease_stages, hc2_disease_stages);
  const leapfrog::TensorMap3<real_type> hc1_cd4_mort = parse_data<real_type>(data, "paed_cd4_mort", hc1_disease_stages, hTM, hc1_age_groups);
  const leapfrog::TensorMap3<real_type> hc2_cd4_mort = parse_data<real_type>(data, "adol_cd4_mort", hc2_disease_stages, hTM, hc2_age_groups);
  const leapfrog::TensorMap1<real_type> hc1_cd4_prog = parse_data<real_type>(data, "paed_cd4_prog", hc1_disease_stages);
  const leapfrog::TensorMap1<real_type> hc2_cd4_prog = parse_data<real_type>(data, "adol_cd4_prog", hc2_disease_stages);
  const leapfrog::TensorMap1<real_type> ctx_effect = parse_data<real_type>(data, "ctx_effect", 1);
  const leapfrog::TensorMap1<real_type> ctx_val = parse_data<real_type>(data, "ctx_val", proj_years);
  leapfrog::Tensor1<real_type> h_art_stage_dur(treatment_stages - 1);
  h_art_stage_dur.setConstant(0.5);

  const leapfrog::Demography<real_type> demography = {
      base_pop,
      survival,
      net_migration,
      age_sex_fertility_ratio,
      births_sex_prop
  };

  const leapfrog::Incidence<real_type> incidence = {
      incidence_rate,
      incidence_relative_risk_age,
      incidence_relative_risk_sex
  };

  const leapfrog::NaturalHistory<real_type> natural_history = {
      cd4_mortality,
      cd4_progression,
      cd4_initdist
  };

  const leapfrog::Art<real_type> art = {
      artcd4elig_idx,
      art_mortality,
      artmx_timerr,
      h_art_stage_dur,
      art_dropout,
      art15plus_num,
      art15plus_isperc
  };

  const leapfrog::Children<real_type> children = {
      hc_nosocomial,
      hc1_cd4_dist,
      hc_cd4_transition
  };

  const leapfrog::Parameters<real_type> params = {options,
                                                  demography,
                                                  incidence,
                                                  natural_history,
                                                  art,
                                                  children};
  return params;
}

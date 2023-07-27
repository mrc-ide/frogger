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
  const int p_fertility_age_groups = options.p_fertility_age_groups;
  const int p_idx_hiv_first_adult = options.p_idx_hiv_first_adult;
  constexpr auto ss = leapfrog::StateSpace<S>();
  constexpr int NS = ss.NS;
  constexpr int pAG = ss.pAG;
  constexpr int hAG = ss.hAG;
  constexpr int hDS = ss.hDS;
  constexpr int hTS = ss.hTS;

  const leapfrog::TensorMap2<real_type> base_pop = parse_data<real_type>(data, "basepop", pAG, NS);
  const leapfrog::TensorMap3<real_type> survival = parse_data<real_type>(data, "Sx", pAG + 1, NS, proj_years);
  const leapfrog::TensorMap3<real_type> net_migration = parse_data<real_type>(data, "netmigr_adj", pAG, NS, proj_years);
  const leapfrog::TensorMap2<real_type> age_sex_fertility_ratio = parse_data<real_type>(data, "asfr", p_fertility_age_groups, proj_years);
  const leapfrog::TensorMap2<real_type> births_sex_prop = parse_data<real_type>(data, "births_sex_prop", NS, proj_years);
  const leapfrog::TensorMap1<real_type> incidence_rate = parse_data<real_type>(data, "incidinput", proj_years);
  const leapfrog::TensorMap3<real_type> incidence_relative_risk_age = parse_data<real_type>(data, "incrr_age", pAG - p_idx_hiv_first_adult, NS, proj_years);
  const leapfrog::TensorMap1<real_type> incidence_relative_risk_sex = parse_data<real_type>(data, "incrr_sex", proj_years);
  const leapfrog::TensorMap3<real_type> cd4_mortality = parse_data<real_type>(data, "cd4_mort", hDS, hAG, NS);
  const leapfrog::TensorMap3<real_type> cd4_progression = parse_data<real_type>(data, "cd4_prog", hDS - 1, hAG, NS);
  const leapfrog::TensorMap1<int> artcd4elig_idx = convert_base<1>(parse_data<int>(data, "artcd4elig_idx", proj_years + 1));
  const leapfrog::TensorMap3<real_type> cd4_initdist = parse_data<real_type>(data, "cd4_initdist", hDS, hAG, NS);
  const leapfrog::TensorMap4<real_type> art_mortality = parse_data<real_type>(data, "art_mort", hTS, hDS, hAG, NS);
  const leapfrog::TensorMap2<real_type> artmx_timerr = parse_data<real_type>(data, "artmx_timerr", hTS, proj_years);
  const leapfrog::TensorMap1<real_type> art_dropout = parse_data<real_type>(data, "art_dropout", proj_years);
  const leapfrog::TensorMap2<real_type> art15plus_num = parse_data<real_type>(data, "art15plus_num", NS, proj_years);
  const leapfrog::TensorMap2<int> art15plus_isperc = parse_data<int>(data, "art15plus_isperc", NS, proj_years);
  leapfrog::Tensor1<real_type> h_art_stage_dur(hTS - 1);
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

  const leapfrog::Parameters<real_type> params = {options,
                                                  demography,
                                                  incidence,
                                                  natural_history,
                                                  art};
  return params;
}

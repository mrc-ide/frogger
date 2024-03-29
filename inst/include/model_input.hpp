// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `model_input.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include <Rcpp.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "types.hpp"
#include "r_utils.hpp"

template<typename ModelVariant, typename real_type>
leapfrog::Parameters<ModelVariant, real_type> setup_model_params(const Rcpp::List &data,
                                                    const leapfrog::Options<real_type> &options,
                                                    const int proj_years) {
  constexpr auto ss = leapfrog::StateSpace<ModelVariant>();

  constexpr auto base = ss.base;
  const leapfrog::TensorMap2<real_type> base_pop = parse_data<real_type>(data, "basepop", base.pAG, base.NS);
  const leapfrog::TensorMap3<real_type> survival_probability = parse_data<real_type>(data, "Sx", base.pAG + 1, base.NS, proj_years);
  const leapfrog::TensorMap3<real_type> net_migration = parse_data<real_type>(data, "netmigr_adj", base.pAG, base.NS, proj_years);
  const leapfrog::TensorMap2<real_type> age_specific_fertility_rate = parse_data<real_type>(data, "asfr", options.p_fertility_age_groups, proj_years);
  const leapfrog::TensorMap2<real_type> births_sex_prop = parse_data<real_type>(data, "births_sex_prop", base.NS, proj_years);
  const leapfrog::TensorMap1<real_type> total_rate = parse_data<real_type>(data, "incidinput", proj_years);
  const leapfrog::TensorMap3<real_type> age_rate_ratio = parse_data<real_type>(data, "incrr_age", base.pAG - options.p_idx_hiv_first_adult, base.NS, proj_years);
  const leapfrog::TensorMap1<real_type> sex_rate_ratio = parse_data<real_type>(data, "incrr_sex", proj_years);
  const leapfrog::TensorMap3<real_type> cd4_mortality = parse_data<real_type>(data, "cd4_mort", base.hDS, base.hAG, base.NS);
  const leapfrog::TensorMap3<real_type> cd4_progression = parse_data<real_type>(data, "cd4_prog", base.hDS - 1, base.hAG, base.NS);
  const leapfrog::Tensor1<int> idx_hm_elig = convert_base<1>(parse_data<int>(data, "artcd4elig_idx", proj_years + 1));
  const leapfrog::TensorMap3<real_type> cd4_initial_distribution = parse_data<real_type>(data, "cd4_initdist", base.hDS, base.hAG, base.NS);
  const leapfrog::TensorMap4<real_type> mortality = parse_data<real_type>(data, "art_mort", base.hTS, base.hDS, base.hAG, base.NS);
  const leapfrog::TensorMap2<real_type> mortaility_time_rate_ratio = parse_data<real_type>(data, "artmx_timerr", base.hTS, proj_years);
  const leapfrog::TensorMap1<real_type> dropout = parse_data<real_type>(data, "art_dropout", proj_years);
  const leapfrog::TensorMap2<real_type> adults_on_art = parse_data<real_type>(data, "art15plus_num", base.NS, proj_years);
  const leapfrog::TensorMap2<int> adults_on_art_is_percent = parse_data<int>(data, "art15plus_isperc", base.NS, proj_years);
  const int scale_cd4_mortality = Rcpp::as<int>(data["scale_cd4_mort"]);
  const real_type initiation_mortality_weight = Rcpp::as<real_type>(data["art_alloc_mxweight"]);
  leapfrog::Tensor1<real_type> h_art_stage_dur(base.hTS - 1);
  h_art_stage_dur.setConstant(0.5);

  const leapfrog::Demography<real_type> demography = {
      base_pop,
      survival_probability,
      net_migration,
      age_specific_fertility_rate,
      births_sex_prop
  };

  const leapfrog::Incidence<real_type> incidence = {
      total_rate,
      age_rate_ratio,
      sex_rate_ratio
  };

  const leapfrog::NaturalHistory<real_type> natural_history = {
      cd4_mortality,
      cd4_progression,
      cd4_initial_distribution,
      scale_cd4_mortality
  };

  const leapfrog::Art<real_type> art = {
      idx_hm_elig,
      mortality,
      mortaility_time_rate_ratio,
      h_art_stage_dur,
      dropout,
      adults_on_art,
      adults_on_art_is_percent,
      initiation_mortality_weight
  };

  const leapfrog::BaseModelParameters<real_type> base_model_params = {
        options,
        demography,
        incidence,
        natural_history,
        art
    };

  if constexpr (ModelVariant::run_child_model) {
    constexpr auto children = ss.children;
    constexpr auto base = ss.base;
    const leapfrog::TensorMap1<real_type> hc_nosocomial = parse_data<real_type>(data, "paed_incid_input", proj_years);
    const leapfrog::TensorMap1<real_type> hc1_cd4_dist = parse_data<real_type>(data, "paed_cd4_dist", children.hc2DS);
    const leapfrog::TensorMap2<real_type> hc_cd4_transition = parse_data<real_type>(data, "paed_cd4_transition", children.hc2DS, children.hc1DS);
    const leapfrog::TensorMap3<real_type> hc1_cd4_mort = parse_data<real_type>(data, "paed_cd4_mort", children.hc1DS, children.hcTT, children.hc1AG);
    const leapfrog::TensorMap3<real_type> hc2_cd4_mort = parse_data<real_type>(data, "adol_cd4_mort", children.hc2DS, children.hcTT, children.hc2AG);
    const leapfrog::TensorMap1<real_type> hc1_cd4_prog = parse_data<real_type>(data, "paed_cd4_prog", children.hc1DS);
    const leapfrog::TensorMap1<real_type> hc2_cd4_prog = parse_data<real_type>(data, "adol_cd4_prog", children.hc2DS);
    const real_type ctx_effect = Rcpp::as<real_type>(data["ctx_effect"]);
    const leapfrog::TensorMap1<real_type> ctx_val = parse_data<real_type>(data, "ctx_val", proj_years);
    const leapfrog::TensorMap1<real_type> hc_art_elig_age = parse_data<real_type>(data, "paed_art_elig_age", proj_years);
    const leapfrog::Tensor2<real_type> hc_art_elig_cd4 = convert_base<2>(parse_data<real_type>(data, "paed_art_elig_cd4", options.p_idx_hiv_first_adult, proj_years));
    const leapfrog::TensorMap3<real_type> hc_art_mort_rr = parse_data<real_type>(data, "mort_art_rr", base.hTS, options.p_idx_hiv_first_adult, proj_years);
    const leapfrog::TensorMap3<real_type> hc1_art_mort = parse_data<real_type>(data, "paed_art_mort", children.hc1DS, base.hTS, children.hc1AG);
    const leapfrog::TensorMap3<real_type> hc2_art_mort = parse_data<real_type>(data, "adol_art_mort", children.hc2DS, base.hTS, children.hc2AG);
    const leapfrog::TensorMap1<int> hc_art_isperc = parse_data<int>(data, "artpaeds_isperc", proj_years);
    const leapfrog::TensorMap1<real_type> hc_art_val = parse_data<real_type>(data, "paed_art_val", proj_years);
    const leapfrog::TensorMap2<real_type> hc_art_init_dist = parse_data<real_type>(data, "init_art_dist", options.p_idx_hiv_first_adult, proj_years);
    const leapfrog::Children<real_type> child = {
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
    const leapfrog::ChildModelParameters<ModelVariant, real_type> child_model_params = {
        child
    };
    return leapfrog::Parameters<ModelVariant, real_type>{
        base_model_params,
        child_model_params
    };
  } else {
    return leapfrog::Parameters<ModelVariant, real_type>{
        base_model_params
    };
  }
}

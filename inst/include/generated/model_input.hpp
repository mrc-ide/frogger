// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `model_input.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include <Rcpp.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "../intermediate_data.hpp"
#include "../r_utils.hpp"


template <typename ModelVariant, typename real_type>
using hiv_params_type = std::conditional_t<
    ModelVariant::run_hiv_simulation,
    leapfrog::HivSimulationParameters<real_type>,
    leapfrog::Empty>;

template <typename ModelVariant, typename real_type>
static hiv_params_type<ModelVariant, real_type> get_hiv_params(const Rcpp::List &data,
                                                               const leapfrog::Options<real_type> &options,
                                                               const int proj_years) {
  if constexpr (ModelVariant::run_hiv_simulation) {
    constexpr auto ss = leapfrog::StateSpace<ModelVariant>();
    constexpr auto dp = ss.dp;
    constexpr auto hiv = ss.hiv;
    const leapfrog::TensorMap1<real_type> total_rate = parse_data<real_type>(data, "incidinput", proj_years);
    const leapfrog::TensorMap3<real_type> relative_risk_age = parse_data<real_type>(data, "incrr_age", dp.pAG - options.p_idx_hiv_first_adult, dp.NS, proj_years);
    const leapfrog::TensorMap1<real_type> relative_risk_sex = parse_data<real_type>(data, "incrr_sex", proj_years);
    const leapfrog::TensorMap3<real_type> cd4_mortality = parse_data<real_type>(data, "cd4_mort", hiv.hDS, hiv.hAG, dp.NS);
    const leapfrog::TensorMap3<real_type> cd4_progression = parse_data<real_type>(data, "cd4_prog", hiv.hDS - 1, hiv.hAG, dp.NS);
    const leapfrog::Tensor1<int> idx_hm_elig = convert_0_based<1>(parse_data<int>(data, "artcd4elig_idx", proj_years));
    const leapfrog::TensorMap3<real_type> cd4_initial_distribution = parse_data<real_type>(data, "cd4_initdist", hiv.hDS, hiv.hAG, dp.NS);
    const leapfrog::TensorMap4<real_type> mortality = parse_data<real_type>(data, "art_mort", hiv.hTS, hiv.hDS, hiv.hAG, dp.NS);
    const leapfrog::TensorMap2<real_type> mortaility_time_rate_ratio = parse_data<real_type>(data, "artmx_timerr", hiv.hTS, proj_years);
    const int dropout_recover_cd4 = Rcpp::as<int>(data["art_dropout_recover_cd4"]);
    const leapfrog::TensorMap1<real_type> dropout_rate = parse_data<real_type>(data, "art_dropout_rate", proj_years);
    const leapfrog::TensorMap2<real_type> adults_on_art = parse_data<real_type>(data, "art15plus_num", dp.NS, proj_years);
    const leapfrog::TensorMap2<int> adults_on_art_is_percent = parse_data<int>(data, "art15plus_isperc", dp.NS, proj_years);
    const int scale_cd4_mortality = Rcpp::as<int>(data["scale_cd4_mort"]);
    const real_type initiation_mortality_weight = Rcpp::as<real_type>(data["art_alloc_mxweight"]);
    leapfrog::Tensor1<real_type> h_art_stage_dur(hiv.hTS - 1);
    h_art_stage_dur.setConstant(0.5);
    const leapfrog::Incidence<real_type> incidence_params = {
        total_rate,
        relative_risk_age,
        relative_risk_sex,
    };
    const leapfrog::NaturalHistory<real_type> natural_history_params = {
        cd4_mortality,
        cd4_progression,
        cd4_initial_distribution,
        scale_cd4_mortality,
    };
    const leapfrog::Art<real_type> art_params = {
        idx_hm_elig,
        mortality,
        mortaility_time_rate_ratio,
        dropout_recover_cd4,
        dropout_rate,
        adults_on_art,
        adults_on_art_is_percent,
        h_art_stage_dur,
        initiation_mortality_weight,
    };
    return leapfrog::HivSimulationParameters<real_type>{
      incidence_params,
      natural_history_params,
      art_params
    };
  } else {
    return leapfrog::Empty{};
  }
}

template <typename ModelVariant, typename real_type>
using child_params_type = std::conditional_t<
    ModelVariant::run_child_model,
    leapfrog::ChildModelParameters<real_type>,
    leapfrog::Empty>;

template <typename ModelVariant, typename real_type>
static child_params_type<ModelVariant, real_type> get_child_params(const Rcpp::List &data,
                                                                 const leapfrog::Options<real_type> &options,
                                                                 const int proj_years) {
  if constexpr (ModelVariant::run_child_model) {
    constexpr auto ss = leapfrog::StateSpace<ModelVariant>();
    constexpr auto children = ss.children;
    constexpr auto dp = ss.dp;
    constexpr auto hiv = ss.hiv;
    const leapfrog::TensorMap1<real_type> hc_nosocomial = parse_data<real_type>(data, "paed_incid_input", proj_years);
    const leapfrog::TensorMap1<real_type> hc1_cd4_dist = parse_data<real_type>(data, "paed_cd4_dist", children.hc2DS);
    const leapfrog::TensorMap2<real_type> hc_cd4_transition = parse_data<real_type>(data, "paed_cd4_transition", children.hc2DS, children.hc1DS);
    const leapfrog::TensorMap3<real_type> hc1_cd4_mort = parse_data<real_type>(data, "paed_cd4_mort", children.hc1DS, children.hcTT, children.hc1AG);
    const leapfrog::TensorMap3<real_type> hc2_cd4_mort = parse_data<real_type>(data, "adol_cd4_mort", children.hc2DS, children.hcTT, children.hc2AG);
    const leapfrog::TensorMap3<real_type> hc1_cd4_prog = parse_data<real_type>(data, "paed_cd4_prog", children.hc1DS, children.hc1AG_c, dp.NS);
    const leapfrog::TensorMap3<real_type> hc2_cd4_prog = parse_data<real_type>(data, "adol_cd4_prog", children.hc2DS, children.hc2AG_c, dp.NS);
    const real_type ctx_effect = Rcpp::as<real_type>(data["ctx_effect"]);
    const leapfrog::TensorMap1<real_type> ctx_val = parse_data<real_type>(data, "ctx_val", proj_years);
    const leapfrog::TensorMap1<int> hc_art_elig_age = parse_data<int>(data, "paed_art_elig_age", proj_years);
    const leapfrog::Tensor2<real_type> hc_art_elig_cd4 = convert_0_based<2>(parse_data<real_type>(data, "paed_art_elig_cd4", options.p_idx_hiv_first_adult, proj_years));
    const leapfrog::TensorMap3<real_type> hc_art_mort_rr = parse_data<real_type>(data, "mort_art_rr", hiv.hTS, options.p_idx_hiv_first_adult, proj_years);
    const leapfrog::TensorMap3<real_type> hc1_art_mort = parse_data<real_type>(data, "paed_art_mort", children.hc1DS, hiv.hTS, children.hc1AG);
    const leapfrog::TensorMap3<real_type> hc2_art_mort = parse_data<real_type>(data, "adol_art_mort", children.hc2DS, hiv.hTS, children.hc2AG);
    const leapfrog::TensorMap1<int> hc_art_isperc = parse_data<int>(data, "artpaeds_isperc", proj_years);
    const leapfrog::TensorMap2<real_type> hc_art_val = parse_data<real_type>(data, "paed_art_val", children.hcAG_coarse, proj_years);
    const leapfrog::TensorMap2<real_type> hc_art_init_dist = parse_data<real_type>(data, "init_art_dist", options.p_idx_hiv_first_adult, proj_years);
    const leapfrog::TensorMap2<real_type> adult_cd4_dist = parse_data<real_type>(data, "adult_cd4_dist", hiv.hDS, children.hc2DS);
    const leapfrog::TensorMap1<real_type> fert_mult_by_age = parse_data<real_type>(data, "fert_mult_by_age", options.p_fertility_age_groups);
    const leapfrog::TensorMap1<real_type> fert_mult_off_art = parse_data<real_type>(data, "fert_mult_offart", hiv.hDS);
    const leapfrog::TensorMap1<real_type> fert_mult_on_art = parse_data<real_type>(data, "fert_mult_onart", options.p_fertility_age_groups);
    const leapfrog::TensorMap1<real_type> total_fertility_rate = parse_data<real_type>(data, "tfr", proj_years);
    const real_type local_adj_factor = Rcpp::as<real_type>(data["laf"]);
    const leapfrog::TensorMap2<real_type> PMTCT = parse_data<real_type>(data, "pmtct", children.hPS, proj_years);
    const leapfrog::TensorMap2<real_type> vertical_transmission_rate = parse_data<real_type>(data, "mtct", hiv.hDS + 1, children.hVT);
    const leapfrog::TensorMap3<real_type> PMTCT_transmission_rate = parse_data<real_type>(data, "pmtct_mtct", hiv.hDS, children.hPS, children.hVT);
    const leapfrog::TensorMap2<real_type> PMTCT_dropout = parse_data<real_type>(data, "pmtct_dropout", children.hPS_dropout, proj_years);
    const leapfrog::TensorMap1<int> PMTCT_input_is_percent = parse_data<int>(data, "pmtct_input_isperc", proj_years);
    const leapfrog::TensorMap2<real_type> breastfeeding_duration_art = parse_data<real_type>(data, "bf_duration_art", children.hBF, proj_years);
    const leapfrog::TensorMap2<real_type> breastfeeding_duration_no_art = parse_data<real_type>(data, "bf_duration_no_art", children.hBF, proj_years);
    const leapfrog::TensorMap1<real_type> mat_hiv_births = parse_data<real_type>(data, "mat_hiv_births", proj_years);
    const leapfrog::TensorMap1<int> mat_prev_input = parse_data<int>(data, "mat_prev_input", proj_years);
    const leapfrog::TensorMap1<real_type> prop_lt200 = parse_data<real_type>(data, "prop_lt200", proj_years);
    const leapfrog::TensorMap1<real_type> prop_gte350 = parse_data<real_type>(data, "prop_gte350", proj_years);
    const leapfrog::TensorMap1<real_type> incrate = parse_data<real_type>(data, "incrate", proj_years);
    const leapfrog::TensorMap1<int> ctx_val_is_percent = parse_data<int>(data, "ctx_val_ispercent", proj_years);
    const leapfrog::TensorMap1<int> hc_art_is_age_spec = parse_data<int>(data, "paed_art_age_spec", proj_years);
    const leapfrog::TensorMap1<real_type> hc_age_coarse = parse_data<real_type>(data, "hc_age_coarse", children.hcAG_end);
    const leapfrog::TensorMap2<real_type> abortion = parse_data<real_type>(data, "abortion", children.hAB_ind, proj_years);
    const leapfrog::TensorMap1<real_type> patients_reallocated = parse_data<real_type>(data, "patients_reallocated", proj_years);
    const leapfrog::TensorMap1<real_type> hc_art_ltfu = parse_data<real_type>(data, "paed_art_ltfu", proj_years);
    const leapfrog::TensorMap1<int> hc_age_coarse_cd4 = parse_data<int>(data, "hc_age_coarse_cd4", options.p_idx_hiv_first_adult);
    const leapfrog::TensorMap2<real_type> adult_female_infections = parse_data<real_type>(data, "adult_female_infections", options.p_fertility_age_groups, proj_years);
    const leapfrog::TensorMap2<real_type> adult_female_hivnpop = parse_data<real_type>(data, "hivnpop", options.p_fertility_age_groups, proj_years);
    const leapfrog::TensorMap1<real_type> total_births = parse_data<real_type>(data, "total_births", proj_years);
    const leapfrog::Children<real_type> children_params = {
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
        hc_art_init_dist,
        adult_cd4_dist,
        fert_mult_by_age,
        fert_mult_off_art,
        fert_mult_on_art,
        total_fertility_rate,
        local_adj_factor,
        PMTCT,
        vertical_transmission_rate,
        PMTCT_transmission_rate,
        PMTCT_dropout,
        PMTCT_input_is_percent,
        breastfeeding_duration_art,
        breastfeeding_duration_no_art,
        mat_hiv_births,
        mat_prev_input,
        prop_lt200,
        prop_gte350,
        incrate,
        ctx_val_is_percent,
        hc_art_is_age_spec,
        hc_age_coarse,
        abortion,
        patients_reallocated,
        hc_art_ltfu,
        hc_age_coarse_cd4,
        adult_female_infections,
        adult_female_hivnpop,
        total_births,
    };
    return leapfrog::ChildModelParameters<real_type> {
      children_params
    };
  } else {
    return leapfrog::Empty{};
  }
}

/**
 * @brief Translate R data structures into C++ eigen tensors for use in model simulation
 *
 * @tparam ModelVariant The variant of the model to be run, used for compile time switching.
 * @tparam real_type The data type used for real numbers in the simulation, usually a double.
 * @param data Rcpp list of input data.
 * @param options Set of runtime model options.
 * @param proj_years Number of years to project.
 * @return An Parameters object containing Eigen Tensors required for the simulation.
 */
template<typename ModelVariant, typename real_type>
leapfrog::Parameters<ModelVariant, real_type> setup_model_params(const Rcpp::List &data,
                                                    const leapfrog::Options<real_type> &options,
                                                    const int proj_years) {
  constexpr auto ss = leapfrog::StateSpace<ModelVariant>();

  constexpr auto dp = ss.dp;
  const leapfrog::TensorMap2<real_type> base_pop = parse_data<real_type>(data, "basepop", dp.pAG, dp.NS);
  const leapfrog::TensorMap3<real_type> survival_probability = parse_data<real_type>(data, "Sx", dp.pAG + 1, dp.NS, proj_years);
  const leapfrog::TensorMap3<real_type> net_migration = parse_data<real_type>(data, "netmigr_adj", dp.pAG, dp.NS, proj_years);
  const leapfrog::TensorMap2<real_type> age_specific_fertility_rate = parse_data<real_type>(data, "asfr", options.p_fertility_age_groups, proj_years);
  const leapfrog::TensorMap2<real_type> births_sex_prop = parse_data<real_type>(data, "births_sex_prop", dp.NS, proj_years);

  const leapfrog::Demography<real_type> demography_params = {
      base_pop,
      survival_probability,
      net_migration,
      age_specific_fertility_rate,
      births_sex_prop,
  };

  const leapfrog::DemographicProjectionParameters<real_type> demog_model_params = {
        demography_params
  };

  auto hiv_params = get_hiv_params<ModelVariant, real_type>(data, options, proj_years);
  auto child_params = get_child_params<ModelVariant, real_type>(data, options, proj_years);

  return leapfrog::Parameters<ModelVariant, real_type>{options, demog_model_params, hiv_params, child_params};
}

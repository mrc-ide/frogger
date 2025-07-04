// Generated by leapfrog cpp_generation: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit leapfrog json files in `cpp_generation/modelSchemas` and run `uv run ./src/main.py` inside
// the `cpp_generation` folder.

#pragma once

#include <filesystem>
#include <string_view>
#include <format>
#include <stdexcept>

#include "../../array/array.h"
#include "../config_mixer.hpp"
#include "c_types.hpp"

namespace leapfrog {
namespace internal {

template<typename T, size_t Rank>
auto read_data(T* data, int length, std::string_view name, nda::shape_of_rank<Rank> shape) {
  const auto size = shape.flat_max() + 1;
  if (length != size) {
    throw std::invalid_argument(
      std::format(
        "Input data '{}' is the wrong size. Received array of length '{}', expected '{}'.",
        name, length, size
      )
    );
  }
  return nda::array_ref_of_rank<T, Rank>(data, shape);
}

template <typename T, typename Shape>
void fill_initial_state(T* data, int length, const std::string_view name, nda::array<T, Shape> array) {
  nda::array_ref<T, Shape> array_ref(data);

  nda::for_each_index(Shape(), [&](auto i) {
    array[i] = array_ref[i];
  });
}

template<typename T, typename Shape>
void write_data(const nda::array<T, Shape>& array, T* output, int length, std::string_view name) {
  const auto* dataPtr = array.data();
  std::size_t totalSize = array.size();

  if (length != totalSize) {
    throw std::invalid_argument(
      std::format(
        "Output data '{}' is the wrong size. Received array of length '{}', expected '{}'.",
        name, length, totalSize
      )
    );
  }
  std::copy(dataPtr, dataPtr + totalSize, output);
}

template<typename real_type, MV ModelVariant>
struct DpAdapter<Language::C, real_type, ModelVariant> {
  using SS = SSMixed<ModelVariant>;
  using Config = DpConfig<real_type, ModelVariant>;

  static Config::Pars get_pars(
    const CParams<real_type> &params,
    const Options<real_type> &opts
  ) {
    return {
      .base_pop = read_data<real_type, 2>(params.dp->base_pop, params.dp->base_pop_length, "base_pop", { nda::dim<>(0, SS::pAG, 1), nda::dim<>(0, SS::NS, (SS::pAG)) }),
      .survival_probability = read_data<real_type, 3>(params.dp->survival_probability, params.dp->survival_probability_length, "survival_probability", { nda::dim<>(0, SS::pAG + 1, 1), nda::dim<>(0, SS::NS, (SS::pAG + 1)), nda::dim<>(0, opts.proj_steps, (SS::pAG + 1) * (SS::NS)) }),
      .net_migration = read_data<real_type, 3>(params.dp->net_migration, params.dp->net_migration_length, "net_migration", { nda::dim<>(0, SS::pAG, 1), nda::dim<>(0, SS::NS, (SS::pAG)), nda::dim<>(0, opts.proj_steps, (SS::pAG) * (SS::NS)) }),
      .age_specific_fertility_rate = read_data<real_type, 2>(params.dp->age_specific_fertility_rate, params.dp->age_specific_fertility_rate_length, "age_specific_fertility_rate", { nda::dim<>(0, SS::p_fertility_age_groups, 1), nda::dim<>(0, opts.proj_steps, (SS::p_fertility_age_groups)) }),
      .births_sex_prop = read_data<real_type, 2>(params.dp->births_sex_prop, params.dp->births_sex_prop_length, "births_sex_prop", { nda::dim<>(0, SS::NS, 1), nda::dim<>(0, opts.proj_steps, (SS::NS)) })
    };
  };

  static Config::State get_initial_state(
    const CState<real_type>& state
  ){
    typename Config::State initial_state;
    fill_initial_state<real_type, typename Config::State::shape_p_total_pop>(state.dp->p_total_pop, state.dp->p_total_pop_length, "p_total_pop", initial_state.p_total_pop);
    fill_initial_state<real_type, typename Config::State::shape_p_total_pop_background_deaths>(state.dp->p_total_pop_background_deaths, state.dp->p_total_pop_background_deaths_length, "p_total_pop_background_deaths", initial_state.p_total_pop_background_deaths);
    initial_state.births = *(state.dp->births);
    return initial_state;
  };

  static constexpr int output_count = 3;

  static int build_output(
    int index,
    const Config::OutputState& state,
    CState<real_type>& out
  ) {
    write_data<real_type, typename Config::OutputState::shape_p_total_pop>(state.p_total_pop, out.dp->p_total_pop, out.dp->p_total_pop_length, "p_total_pop");
    write_data<real_type, typename Config::OutputState::shape_p_total_pop_background_deaths>(state.p_total_pop_background_deaths, out.dp->p_total_pop_background_deaths, out.dp->p_total_pop_background_deaths_length, "p_total_pop_background_deaths");
    write_data<real_type, typename Config::OutputState::shape_births>(state.births, out.dp->births, out.dp->births_length, "births");
    return index + output_count;
  };

  static int build_output_single_year(
    int index,
    const Config::State& state,
    CState<real_type>& out
  ) {
    write_data<real_type, typename Config::State::shape_p_total_pop>(state.p_total_pop, out.dp->p_total_pop, out.dp->p_total_pop_length, "p_total_pop");
    write_data<real_type, typename Config::State::shape_p_total_pop_background_deaths>(state.p_total_pop_background_deaths, out.dp->p_total_pop_background_deaths, out.dp->p_total_pop_background_deaths_length, "p_total_pop_background_deaths");
    *(out.dp->births) = state.births;
    return index + output_count;
  };
};

template<typename real_type, MV ModelVariant>
struct HaAdapter<Language::C, real_type, ModelVariant> {
  using SS = SSMixed<ModelVariant>;
  using Config = HaConfig<real_type, ModelVariant>;

  static Config::Pars get_pars(
    const CParams<real_type> &params,
    const Options<real_type> &opts
  ) {
    return {
      .total_rate = read_data<real_type, 1>(params.ha->total_rate, params.ha->total_rate_length, "total_rate", { nda::dim<>(0, opts.proj_steps, 1) }),
      .relative_risk_age = read_data<real_type, 3>(params.ha->relative_risk_age, params.ha->relative_risk_age_length, "relative_risk_age", { nda::dim<>(0, SS::pAG - SS::p_idx_hiv_first_adult, 1), nda::dim<>(0, SS::NS, (SS::pAG - SS::p_idx_hiv_first_adult)), nda::dim<>(0, opts.proj_steps, (SS::pAG - SS::p_idx_hiv_first_adult) * (SS::NS)) }),
      .relative_risk_sex = read_data<real_type, 1>(params.ha->relative_risk_sex, params.ha->relative_risk_sex_length, "relative_risk_sex", { nda::dim<>(0, opts.proj_steps, 1) }),
      .cd4_mortality = read_data<real_type, 3>(params.ha->cd4_mortality, params.ha->cd4_mortality_length, "cd4_mortality", { nda::dim<>(0, SS::hDS, 1), nda::dim<>(0, SS::hAG, (SS::hDS)), nda::dim<>(0, SS::NS, (SS::hDS) * (SS::hAG)) }),
      .cd4_progression = read_data<real_type, 3>(params.ha->cd4_progression, params.ha->cd4_progression_length, "cd4_progression", { nda::dim<>(0, SS::hDS - 1, 1), nda::dim<>(0, SS::hAG, (SS::hDS - 1)), nda::dim<>(0, SS::NS, (SS::hDS - 1) * (SS::hAG)) }),
      .cd4_initial_distribution = read_data<real_type, 3>(params.ha->cd4_initial_distribution, params.ha->cd4_initial_distribution_length, "cd4_initial_distribution", { nda::dim<>(0, SS::hDS, 1), nda::dim<>(0, SS::hAG, (SS::hDS)), nda::dim<>(0, SS::NS, (SS::hDS) * (SS::hAG)) }),
      .scale_cd4_mortality = params.ha->scale_cd4_mortality,
      .idx_hm_elig = read_data<int, 1>(params.ha->idx_hm_elig, params.ha->idx_hm_elig_length, "idx_hm_elig", { nda::dim<>(0, opts.proj_steps, 1) }),
      .mortality = read_data<real_type, 4>(params.ha->mortality, params.ha->mortality_length, "mortality", { nda::dim<>(0, SS::hTS, 1), nda::dim<>(0, SS::hDS, (SS::hTS)), nda::dim<>(0, SS::hAG, (SS::hTS) * (SS::hDS)), nda::dim<>(0, SS::NS, (SS::hTS) * (SS::hDS) * (SS::hAG)) }),
      .mortality_time_rate_ratio = read_data<real_type, 2>(params.ha->mortality_time_rate_ratio, params.ha->mortality_time_rate_ratio_length, "mortality_time_rate_ratio", { nda::dim<>(0, SS::hTS, 1), nda::dim<>(0, opts.proj_steps, (SS::hTS)) }),
      .dropout_recover_cd4 = params.ha->dropout_recover_cd4,
      .dropout_rate = read_data<real_type, 1>(params.ha->dropout_rate, params.ha->dropout_rate_length, "dropout_rate", { nda::dim<>(0, opts.proj_steps, 1) }),
      .adults_on_art = read_data<real_type, 2>(params.ha->adults_on_art, params.ha->adults_on_art_length, "adults_on_art", { nda::dim<>(0, SS::NS, 1), nda::dim<>(0, opts.proj_steps, (SS::NS)) }),
      .adults_on_art_is_percent = read_data<int, 2>(params.ha->adults_on_art_is_percent, params.ha->adults_on_art_is_percent_length, "adults_on_art_is_percent", { nda::dim<>(0, SS::NS, 1), nda::dim<>(0, opts.proj_steps, (SS::NS)) }),
      .initiation_mortality_weight = params.ha->initiation_mortality_weight,
      .h_art_stage_dur = read_data<real_type, 1>(params.ha->h_art_stage_dur, params.ha->h_art_stage_dur_length, "h_art_stage_dur", { nda::dim<>(0, SS::hTS - 1, 1) })
    };
  };

  static Config::State get_initial_state(
    const CState<real_type>& state
  ){
    typename Config::State initial_state;
    fill_initial_state<real_type, typename Config::State::shape_p_hiv_pop>(state.ha->p_hiv_pop, state.ha->p_hiv_pop_length, "p_hiv_pop", initial_state.p_hiv_pop);
    fill_initial_state<real_type, typename Config::State::shape_p_hiv_pop_background_deaths>(state.ha->p_hiv_pop_background_deaths, state.ha->p_hiv_pop_background_deaths_length, "p_hiv_pop_background_deaths", initial_state.p_hiv_pop_background_deaths);
    fill_initial_state<real_type, typename Config::State::shape_h_hiv_adult>(state.ha->h_hiv_adult, state.ha->h_hiv_adult_length, "h_hiv_adult", initial_state.h_hiv_adult);
    fill_initial_state<real_type, typename Config::State::shape_h_art_adult>(state.ha->h_art_adult, state.ha->h_art_adult_length, "h_art_adult", initial_state.h_art_adult);
    fill_initial_state<real_type, typename Config::State::shape_h_hiv_deaths_no_art>(state.ha->h_hiv_deaths_no_art, state.ha->h_hiv_deaths_no_art_length, "h_hiv_deaths_no_art", initial_state.h_hiv_deaths_no_art);
    fill_initial_state<real_type, typename Config::State::shape_p_infections>(state.ha->p_infections, state.ha->p_infections_length, "p_infections", initial_state.p_infections);
    fill_initial_state<real_type, typename Config::State::shape_h_hiv_deaths_art>(state.ha->h_hiv_deaths_art, state.ha->h_hiv_deaths_art_length, "h_hiv_deaths_art", initial_state.h_hiv_deaths_art);
    fill_initial_state<real_type, typename Config::State::shape_h_art_initiation>(state.ha->h_art_initiation, state.ha->h_art_initiation_length, "h_art_initiation", initial_state.h_art_initiation);
    fill_initial_state<real_type, typename Config::State::shape_p_hiv_deaths>(state.ha->p_hiv_deaths, state.ha->p_hiv_deaths_length, "p_hiv_deaths", initial_state.p_hiv_deaths);
    return initial_state;
  };

  static constexpr int output_count = 9;

  static int build_output(
    int index,
    const Config::OutputState& state,
    CState<real_type>& out
  ) {
    write_data<real_type, typename Config::OutputState::shape_p_hiv_pop>(state.p_hiv_pop, out.ha->p_hiv_pop, out.ha->p_hiv_pop_length, "p_hiv_pop");
    write_data<real_type, typename Config::OutputState::shape_p_hiv_pop_background_deaths>(state.p_hiv_pop_background_deaths, out.ha->p_hiv_pop_background_deaths, out.ha->p_hiv_pop_background_deaths_length, "p_hiv_pop_background_deaths");
    write_data<real_type, typename Config::OutputState::shape_h_hiv_adult>(state.h_hiv_adult, out.ha->h_hiv_adult, out.ha->h_hiv_adult_length, "h_hiv_adult");
    write_data<real_type, typename Config::OutputState::shape_h_art_adult>(state.h_art_adult, out.ha->h_art_adult, out.ha->h_art_adult_length, "h_art_adult");
    write_data<real_type, typename Config::OutputState::shape_h_hiv_deaths_no_art>(state.h_hiv_deaths_no_art, out.ha->h_hiv_deaths_no_art, out.ha->h_hiv_deaths_no_art_length, "h_hiv_deaths_no_art");
    write_data<real_type, typename Config::OutputState::shape_p_infections>(state.p_infections, out.ha->p_infections, out.ha->p_infections_length, "p_infections");
    write_data<real_type, typename Config::OutputState::shape_h_hiv_deaths_art>(state.h_hiv_deaths_art, out.ha->h_hiv_deaths_art, out.ha->h_hiv_deaths_art_length, "h_hiv_deaths_art");
    write_data<real_type, typename Config::OutputState::shape_h_art_initiation>(state.h_art_initiation, out.ha->h_art_initiation, out.ha->h_art_initiation_length, "h_art_initiation");
    write_data<real_type, typename Config::OutputState::shape_p_hiv_deaths>(state.p_hiv_deaths, out.ha->p_hiv_deaths, out.ha->p_hiv_deaths_length, "p_hiv_deaths");
    return index + output_count;
  };

  static int build_output_single_year(
    int index,
    const Config::State& state,
    CState<real_type>& out
  ) {
    write_data<real_type, typename Config::State::shape_p_hiv_pop>(state.p_hiv_pop, out.ha->p_hiv_pop, out.ha->p_hiv_pop_length, "p_hiv_pop");
    write_data<real_type, typename Config::State::shape_p_hiv_pop_background_deaths>(state.p_hiv_pop_background_deaths, out.ha->p_hiv_pop_background_deaths, out.ha->p_hiv_pop_background_deaths_length, "p_hiv_pop_background_deaths");
    write_data<real_type, typename Config::State::shape_h_hiv_adult>(state.h_hiv_adult, out.ha->h_hiv_adult, out.ha->h_hiv_adult_length, "h_hiv_adult");
    write_data<real_type, typename Config::State::shape_h_art_adult>(state.h_art_adult, out.ha->h_art_adult, out.ha->h_art_adult_length, "h_art_adult");
    write_data<real_type, typename Config::State::shape_h_hiv_deaths_no_art>(state.h_hiv_deaths_no_art, out.ha->h_hiv_deaths_no_art, out.ha->h_hiv_deaths_no_art_length, "h_hiv_deaths_no_art");
    write_data<real_type, typename Config::State::shape_p_infections>(state.p_infections, out.ha->p_infections, out.ha->p_infections_length, "p_infections");
    write_data<real_type, typename Config::State::shape_h_hiv_deaths_art>(state.h_hiv_deaths_art, out.ha->h_hiv_deaths_art, out.ha->h_hiv_deaths_art_length, "h_hiv_deaths_art");
    write_data<real_type, typename Config::State::shape_h_art_initiation>(state.h_art_initiation, out.ha->h_art_initiation, out.ha->h_art_initiation_length, "h_art_initiation");
    write_data<real_type, typename Config::State::shape_p_hiv_deaths>(state.p_hiv_deaths, out.ha->p_hiv_deaths, out.ha->p_hiv_deaths_length, "p_hiv_deaths");
    return index + output_count;
  };
};

template<typename real_type, MV ModelVariant>
struct HcAdapter<Language::C, real_type, ModelVariant> {
  using SS = SSMixed<ModelVariant>;
  using Config = HcConfig<real_type, ModelVariant>;

  static Config::Pars get_pars(
    const CParams<real_type> &params,
    const Options<real_type> &opts
  ) {
    return {
      .hc_nosocomial = read_data<real_type, 1>(params.hc->hc_nosocomial, params.hc->hc_nosocomial_length, "hc_nosocomial", { nda::dim<>(0, opts.proj_steps, 1) }),
      .hc1_cd4_dist = read_data<real_type, 1>(params.hc->hc1_cd4_dist, params.hc->hc1_cd4_dist_length, "hc1_cd4_dist", { nda::dim<>(0, SS::hc1DS, 1) }),
      .hc1_cd4_mort = read_data<real_type, 3>(params.hc->hc1_cd4_mort, params.hc->hc1_cd4_mort_length, "hc1_cd4_mort", { nda::dim<>(0, SS::hc1DS, 1), nda::dim<>(0, SS::hcTT, (SS::hc1DS)), nda::dim<>(0, SS::hc1AG, (SS::hc1DS) * (SS::hcTT)) }),
      .hc2_cd4_mort = read_data<real_type, 3>(params.hc->hc2_cd4_mort, params.hc->hc2_cd4_mort_length, "hc2_cd4_mort", { nda::dim<>(0, SS::hc2DS, 1), nda::dim<>(0, SS::hcTT, (SS::hc2DS)), nda::dim<>(0, SS::hc2AG, (SS::hc2DS) * (SS::hcTT)) }),
      .hc1_cd4_prog = read_data<real_type, 3>(params.hc->hc1_cd4_prog, params.hc->hc1_cd4_prog_length, "hc1_cd4_prog", { nda::dim<>(0, SS::hc1DS, 1), nda::dim<>(0, SS::hc1AG_c, (SS::hc1DS)), nda::dim<>(0, SS::NS, (SS::hc1DS) * (SS::hc1AG_c)) }),
      .hc2_cd4_prog = read_data<real_type, 3>(params.hc->hc2_cd4_prog, params.hc->hc2_cd4_prog_length, "hc2_cd4_prog", { nda::dim<>(0, SS::hc2DS, 1), nda::dim<>(0, SS::hc2AG_c, (SS::hc2DS)), nda::dim<>(0, SS::NS, (SS::hc2DS) * (SS::hc2AG_c)) }),
      .ctx_val = read_data<real_type, 1>(params.hc->ctx_val, params.hc->ctx_val_length, "ctx_val", { nda::dim<>(0, opts.proj_steps, 1) }),
      .hc_art_elig_age = read_data<int, 1>(params.hc->hc_art_elig_age, params.hc->hc_art_elig_age_length, "hc_art_elig_age", { nda::dim<>(0, opts.proj_steps, 1) }),
      .hc_art_elig_cd4 = read_data<int, 2>(params.hc->hc_art_elig_cd4, params.hc->hc_art_elig_cd4_length, "hc_art_elig_cd4", { nda::dim<>(0, SS::p_idx_hiv_first_adult, 1), nda::dim<>(0, opts.proj_steps, (SS::p_idx_hiv_first_adult)) }),
      .hc_art_mort_rr = read_data<real_type, 3>(params.hc->hc_art_mort_rr, params.hc->hc_art_mort_rr_length, "hc_art_mort_rr", { nda::dim<>(0, SS::hTS, 1), nda::dim<>(0, SS::p_idx_hiv_first_adult, (SS::hTS)), nda::dim<>(0, opts.proj_steps, (SS::hTS) * (SS::p_idx_hiv_first_adult)) }),
      .hc1_art_mort = read_data<real_type, 3>(params.hc->hc1_art_mort, params.hc->hc1_art_mort_length, "hc1_art_mort", { nda::dim<>(0, SS::hc1DS, 1), nda::dim<>(0, SS::hTS, (SS::hc1DS)), nda::dim<>(0, SS::hc1AG, (SS::hc1DS) * (SS::hTS)) }),
      .hc2_art_mort = read_data<real_type, 3>(params.hc->hc2_art_mort, params.hc->hc2_art_mort_length, "hc2_art_mort", { nda::dim<>(0, SS::hc2DS, 1), nda::dim<>(0, SS::hTS, (SS::hc2DS)), nda::dim<>(0, SS::hc2AG, (SS::hc2DS) * (SS::hTS)) }),
      .hc_art_isperc = read_data<int, 1>(params.hc->hc_art_isperc, params.hc->hc_art_isperc_length, "hc_art_isperc", { nda::dim<>(0, opts.proj_steps, 1) }),
      .hc_art_val = read_data<real_type, 2>(params.hc->hc_art_val, params.hc->hc_art_val_length, "hc_art_val", { nda::dim<>(0, SS::hcAG_coarse, 1), nda::dim<>(0, opts.proj_steps, (SS::hcAG_coarse)) }),
      .hc_art_init_dist = read_data<real_type, 2>(params.hc->hc_art_init_dist, params.hc->hc_art_init_dist_length, "hc_art_init_dist", { nda::dim<>(0, SS::p_idx_hiv_first_adult, 1), nda::dim<>(0, opts.proj_steps, (SS::p_idx_hiv_first_adult)) }),
      .fert_mult_by_age = read_data<real_type, 2>(params.hc->fert_mult_by_age, params.hc->fert_mult_by_age_length, "fert_mult_by_age", { nda::dim<>(0, SS::hAG_fert, 1), nda::dim<>(0, opts.proj_steps, (SS::hAG_fert)) }),
      .fert_mult_off_art = read_data<real_type, 1>(params.hc->fert_mult_off_art, params.hc->fert_mult_off_art_length, "fert_mult_off_art", { nda::dim<>(0, SS::hDS, 1) }),
      .fert_mult_on_art = read_data<real_type, 1>(params.hc->fert_mult_on_art, params.hc->fert_mult_on_art_length, "fert_mult_on_art", { nda::dim<>(0, SS::hAG_fert, 1) }),
      .total_fertility_rate = read_data<real_type, 1>(params.hc->total_fertility_rate, params.hc->total_fertility_rate_length, "total_fertility_rate", { nda::dim<>(0, opts.proj_steps, 1) }),
      .PMTCT = read_data<real_type, 2>(params.hc->PMTCT, params.hc->PMTCT_length, "PMTCT", { nda::dim<>(0, SS::hPS, 1), nda::dim<>(0, opts.proj_steps, (SS::hPS)) }),
      .vertical_transmission_rate = read_data<real_type, 2>(params.hc->vertical_transmission_rate, params.hc->vertical_transmission_rate_length, "vertical_transmission_rate", { nda::dim<>(0, SS::hDS + 1, 1), nda::dim<>(0, SS::hVT, (SS::hDS + 1)) }),
      .PMTCT_transmission_rate = read_data<real_type, 3>(params.hc->PMTCT_transmission_rate, params.hc->PMTCT_transmission_rate_length, "PMTCT_transmission_rate", { nda::dim<>(0, SS::hDS, 1), nda::dim<>(0, SS::hPS, (SS::hDS)), nda::dim<>(0, SS::hVT, (SS::hDS) * (SS::hPS)) }),
      .PMTCT_dropout = read_data<real_type, 2>(params.hc->PMTCT_dropout, params.hc->PMTCT_dropout_length, "PMTCT_dropout", { nda::dim<>(0, SS::hPS_dropout, 1), nda::dim<>(0, opts.proj_steps, (SS::hPS_dropout)) }),
      .PMTCT_input_is_percent = read_data<int, 1>(params.hc->PMTCT_input_is_percent, params.hc->PMTCT_input_is_percent_length, "PMTCT_input_is_percent", { nda::dim<>(0, opts.proj_steps, 1) }),
      .breastfeeding_duration_art = read_data<real_type, 2>(params.hc->breastfeeding_duration_art, params.hc->breastfeeding_duration_art_length, "breastfeeding_duration_art", { nda::dim<>(0, SS::hBF, 1), nda::dim<>(0, opts.proj_steps, (SS::hBF)) }),
      .breastfeeding_duration_no_art = read_data<real_type, 2>(params.hc->breastfeeding_duration_no_art, params.hc->breastfeeding_duration_no_art_length, "breastfeeding_duration_no_art", { nda::dim<>(0, SS::hBF, 1), nda::dim<>(0, opts.proj_steps, (SS::hBF)) }),
      .mat_hiv_births = read_data<real_type, 1>(params.hc->mat_hiv_births, params.hc->mat_hiv_births_length, "mat_hiv_births", { nda::dim<>(0, opts.proj_steps, 1) }),
      .mat_prev_input = read_data<int, 1>(params.hc->mat_prev_input, params.hc->mat_prev_input_length, "mat_prev_input", { nda::dim<>(0, opts.proj_steps, 1) }),
      .prop_lt200 = read_data<real_type, 1>(params.hc->prop_lt200, params.hc->prop_lt200_length, "prop_lt200", { nda::dim<>(0, opts.proj_steps, 1) }),
      .prop_gte350 = read_data<real_type, 1>(params.hc->prop_gte350, params.hc->prop_gte350_length, "prop_gte350", { nda::dim<>(0, opts.proj_steps, 1) }),
      .ctx_val_is_percent = read_data<int, 1>(params.hc->ctx_val_is_percent, params.hc->ctx_val_is_percent_length, "ctx_val_is_percent", { nda::dim<>(0, opts.proj_steps, 1) }),
      .hc_art_is_age_spec = read_data<int, 1>(params.hc->hc_art_is_age_spec, params.hc->hc_art_is_age_spec_length, "hc_art_is_age_spec", { nda::dim<>(0, opts.proj_steps, 1) }),
      .abortion = read_data<real_type, 2>(params.hc->abortion, params.hc->abortion_length, "abortion", { nda::dim<>(0, SS::hAB_ind, 1), nda::dim<>(0, opts.proj_steps, (SS::hAB_ind)) }),
      .patients_reallocated = read_data<real_type, 1>(params.hc->patients_reallocated, params.hc->patients_reallocated_length, "patients_reallocated", { nda::dim<>(0, opts.proj_steps, 1) }),
      .hc_art_ltfu = read_data<real_type, 1>(params.hc->hc_art_ltfu, params.hc->hc_art_ltfu_length, "hc_art_ltfu", { nda::dim<>(0, opts.proj_steps, 1) }),
      .adult_female_infections = read_data<real_type, 2>(params.hc->adult_female_infections, params.hc->adult_female_infections_length, "adult_female_infections", { nda::dim<>(0, SS::p_fertility_age_groups, 1), nda::dim<>(0, opts.proj_steps, (SS::p_fertility_age_groups)) }),
      .adult_female_hivnpop = read_data<real_type, 2>(params.hc->adult_female_hivnpop, params.hc->adult_female_hivnpop_length, "adult_female_hivnpop", { nda::dim<>(0, SS::p_fertility_age_groups, 1), nda::dim<>(0, opts.proj_steps, (SS::p_fertility_age_groups)) }),
      .total_births = read_data<real_type, 1>(params.hc->total_births, params.hc->total_births_length, "total_births", { nda::dim<>(0, opts.proj_steps, 1) }),
      .ctx_effect = read_data<real_type, 1>(params.hc->ctx_effect, params.hc->ctx_effect_length, "ctx_effect", { nda::dim<>(0, 3, 1) }),
      .hc_art_start = params.hc->hc_art_start,
      .local_adj_factor = params.hc->local_adj_factor
    };
  };

  static Config::State get_initial_state(
    const CState<real_type>& state
  ){
    typename Config::State initial_state;
    fill_initial_state<real_type, typename Config::State::shape_hc1_hiv_pop>(state.hc->hc1_hiv_pop, state.hc->hc1_hiv_pop_length, "hc1_hiv_pop", initial_state.hc1_hiv_pop);
    fill_initial_state<real_type, typename Config::State::shape_hc2_hiv_pop>(state.hc->hc2_hiv_pop, state.hc->hc2_hiv_pop_length, "hc2_hiv_pop", initial_state.hc2_hiv_pop);
    fill_initial_state<real_type, typename Config::State::shape_hc1_art_pop>(state.hc->hc1_art_pop, state.hc->hc1_art_pop_length, "hc1_art_pop", initial_state.hc1_art_pop);
    fill_initial_state<real_type, typename Config::State::shape_hc2_art_pop>(state.hc->hc2_art_pop, state.hc->hc2_art_pop_length, "hc2_art_pop", initial_state.hc2_art_pop);
    fill_initial_state<real_type, typename Config::State::shape_hc1_noart_aids_deaths>(state.hc->hc1_noart_aids_deaths, state.hc->hc1_noart_aids_deaths_length, "hc1_noart_aids_deaths", initial_state.hc1_noart_aids_deaths);
    fill_initial_state<real_type, typename Config::State::shape_hc2_noart_aids_deaths>(state.hc->hc2_noart_aids_deaths, state.hc->hc2_noart_aids_deaths_length, "hc2_noart_aids_deaths", initial_state.hc2_noart_aids_deaths);
    fill_initial_state<real_type, typename Config::State::shape_hc1_art_aids_deaths>(state.hc->hc1_art_aids_deaths, state.hc->hc1_art_aids_deaths_length, "hc1_art_aids_deaths", initial_state.hc1_art_aids_deaths);
    fill_initial_state<real_type, typename Config::State::shape_hc2_art_aids_deaths>(state.hc->hc2_art_aids_deaths, state.hc->hc2_art_aids_deaths_length, "hc2_art_aids_deaths", initial_state.hc2_art_aids_deaths);
    fill_initial_state<real_type, typename Config::State::shape_hc_art_init>(state.hc->hc_art_init, state.hc->hc_art_init_length, "hc_art_init", initial_state.hc_art_init);
    fill_initial_state<real_type, typename Config::State::shape_hc_art_need_init>(state.hc->hc_art_need_init, state.hc->hc_art_need_init_length, "hc_art_need_init", initial_state.hc_art_need_init);
    initial_state.hiv_births = *(state.hc->hiv_births);
    initial_state.ctx_need = *(state.hc->ctx_need);
    fill_initial_state<real_type, typename Config::State::shape_infection_by_type>(state.hc->infection_by_type, state.hc->infection_by_type_length, "infection_by_type", initial_state.infection_by_type);
    return initial_state;
  };

  static constexpr int output_count = 13;

  static int build_output(
    int index,
    const Config::OutputState& state,
    CState<real_type>& out
  ) {
    write_data<real_type, typename Config::OutputState::shape_hc1_hiv_pop>(state.hc1_hiv_pop, out.hc->hc1_hiv_pop, out.hc->hc1_hiv_pop_length, "hc1_hiv_pop");
    write_data<real_type, typename Config::OutputState::shape_hc2_hiv_pop>(state.hc2_hiv_pop, out.hc->hc2_hiv_pop, out.hc->hc2_hiv_pop_length, "hc2_hiv_pop");
    write_data<real_type, typename Config::OutputState::shape_hc1_art_pop>(state.hc1_art_pop, out.hc->hc1_art_pop, out.hc->hc1_art_pop_length, "hc1_art_pop");
    write_data<real_type, typename Config::OutputState::shape_hc2_art_pop>(state.hc2_art_pop, out.hc->hc2_art_pop, out.hc->hc2_art_pop_length, "hc2_art_pop");
    write_data<real_type, typename Config::OutputState::shape_hc1_noart_aids_deaths>(state.hc1_noart_aids_deaths, out.hc->hc1_noart_aids_deaths, out.hc->hc1_noart_aids_deaths_length, "hc1_noart_aids_deaths");
    write_data<real_type, typename Config::OutputState::shape_hc2_noart_aids_deaths>(state.hc2_noart_aids_deaths, out.hc->hc2_noart_aids_deaths, out.hc->hc2_noart_aids_deaths_length, "hc2_noart_aids_deaths");
    write_data<real_type, typename Config::OutputState::shape_hc1_art_aids_deaths>(state.hc1_art_aids_deaths, out.hc->hc1_art_aids_deaths, out.hc->hc1_art_aids_deaths_length, "hc1_art_aids_deaths");
    write_data<real_type, typename Config::OutputState::shape_hc2_art_aids_deaths>(state.hc2_art_aids_deaths, out.hc->hc2_art_aids_deaths, out.hc->hc2_art_aids_deaths_length, "hc2_art_aids_deaths");
    write_data<real_type, typename Config::OutputState::shape_hc_art_init>(state.hc_art_init, out.hc->hc_art_init, out.hc->hc_art_init_length, "hc_art_init");
    write_data<real_type, typename Config::OutputState::shape_hc_art_need_init>(state.hc_art_need_init, out.hc->hc_art_need_init, out.hc->hc_art_need_init_length, "hc_art_need_init");
    write_data<real_type, typename Config::OutputState::shape_hiv_births>(state.hiv_births, out.hc->hiv_births, out.hc->hiv_births_length, "hiv_births");
    write_data<real_type, typename Config::OutputState::shape_ctx_need>(state.ctx_need, out.hc->ctx_need, out.hc->ctx_need_length, "ctx_need");
    write_data<real_type, typename Config::OutputState::shape_infection_by_type>(state.infection_by_type, out.hc->infection_by_type, out.hc->infection_by_type_length, "infection_by_type");
    return index + output_count;
  };

  static int build_output_single_year(
    int index,
    const Config::State& state,
    CState<real_type>& out
  ) {
    write_data<real_type, typename Config::State::shape_hc1_hiv_pop>(state.hc1_hiv_pop, out.hc->hc1_hiv_pop, out.hc->hc1_hiv_pop_length, "hc1_hiv_pop");
    write_data<real_type, typename Config::State::shape_hc2_hiv_pop>(state.hc2_hiv_pop, out.hc->hc2_hiv_pop, out.hc->hc2_hiv_pop_length, "hc2_hiv_pop");
    write_data<real_type, typename Config::State::shape_hc1_art_pop>(state.hc1_art_pop, out.hc->hc1_art_pop, out.hc->hc1_art_pop_length, "hc1_art_pop");
    write_data<real_type, typename Config::State::shape_hc2_art_pop>(state.hc2_art_pop, out.hc->hc2_art_pop, out.hc->hc2_art_pop_length, "hc2_art_pop");
    write_data<real_type, typename Config::State::shape_hc1_noart_aids_deaths>(state.hc1_noart_aids_deaths, out.hc->hc1_noart_aids_deaths, out.hc->hc1_noart_aids_deaths_length, "hc1_noart_aids_deaths");
    write_data<real_type, typename Config::State::shape_hc2_noart_aids_deaths>(state.hc2_noart_aids_deaths, out.hc->hc2_noart_aids_deaths, out.hc->hc2_noart_aids_deaths_length, "hc2_noart_aids_deaths");
    write_data<real_type, typename Config::State::shape_hc1_art_aids_deaths>(state.hc1_art_aids_deaths, out.hc->hc1_art_aids_deaths, out.hc->hc1_art_aids_deaths_length, "hc1_art_aids_deaths");
    write_data<real_type, typename Config::State::shape_hc2_art_aids_deaths>(state.hc2_art_aids_deaths, out.hc->hc2_art_aids_deaths, out.hc->hc2_art_aids_deaths_length, "hc2_art_aids_deaths");
    write_data<real_type, typename Config::State::shape_hc_art_init>(state.hc_art_init, out.hc->hc_art_init, out.hc->hc_art_init_length, "hc_art_init");
    write_data<real_type, typename Config::State::shape_hc_art_need_init>(state.hc_art_need_init, out.hc->hc_art_need_init, out.hc->hc_art_need_init_length, "hc_art_need_init");
    *(out.hc->hiv_births) = state.hiv_births;
    *(out.hc->ctx_need) = state.ctx_need;
    write_data<real_type, typename Config::State::shape_infection_by_type>(state.infection_by_type, out.hc->infection_by_type, out.hc->infection_by_type_length, "infection_by_type");
    return index + output_count;
  };
};

} // namespace internal
} // namespace leapfrog

#pragma once

#include <filesystem>
#include <string_view>
#include <format>

#include "../config.hpp"
#include "../../serialize_eigen.hpp"

namespace leapfrog {
namespace internal {

template<typename T, typename... Args>
auto read_data(const std::filesystem::path &input_dir, std::string_view key, Args... dims) {
  if (!std::filesystem::exists(input_dir / key)) {
    throw std::runtime_error(std::format("File '{}' in input dir '{}' does not exist.\n", key, input_dir.string()));
  }

  constexpr std::size_t rank = sizeof...(dims);
  if constexpr (rank == 0) {
    return serialize::deserialize_scalar<T>(input_dir / key);
  } else {
    return serialize::deserialize_tensor<T, rank>(input_dir / key);
  }
}


template<typename real_type, MV ModelVariant>
struct DpAdapterCpp {
  using SS = SSMixed<ModelVariant>;
  using Config = DpConfig<real_type, ModelVariant>;

  static Config::Pars get_pars(
    const std::filesystem::path &input_dir,
    const Opts<real_type> &opts,
    const int proj_years
  ) {
    return {
      .base_pop = read_data<real_type>(input_dir, "base_pop", SS::pAG, SS::NS),
      .survival_probability = read_data<real_type>(input_dir, "survival_probability", SS::pAG + 1, SS::NS, proj_years),
      .net_migration = read_data<real_type>(input_dir, "net_migration", SS::pAG, SS::NS, proj_years),
      .age_specific_fertility_rate = read_data<real_type>(input_dir, "age_specific_fertility_rate", opts.p_fertility_age_groups, proj_years),
      .births_sex_prop = read_data<real_type>(input_dir, "births_sex_prop", SS::NS, proj_years)
    };
  };

  static void write_output(
    std::filesystem::path& output_dir,
    const Config::OutputState& state
  ) {
    serialize::serialize_tensor<double, 3>(state.p_total_pop, output_dir / "p_total_pop");
    serialize::serialize_tensor<double, 3>(state.p_total_pop_natural_deaths, output_dir / "p_total_pop_natural_deaths");
    serialize::serialize_tensor<double, 1>(state.births, output_dir / "births");
  };
};

template<typename real_type, MV ModelVariant>
struct HaAdapterCpp {
  using SS = SSMixed<ModelVariant>;
  using Config = HaConfig<real_type, ModelVariant>;

  static Config::Pars get_pars(
    const std::filesystem::path &input_dir,
    const Opts<real_type> &opts,
    const int proj_years
  ) {
    return {
      .total_rate = read_data<real_type>(input_dir, "total_rate", proj_years),
      .relative_risk_age = read_data<real_type>(input_dir, "relative_risk_age", SS::pAG - opts.p_idx_hiv_first_adult, SS::NS, proj_years),
      .relative_risk_sex = read_data<real_type>(input_dir, "relative_risk_sex", proj_years),
      .cd4_mortality = read_data<real_type>(input_dir, "cd4_mortality", SS::hDS, SS::hAG, SS::NS),
      .cd4_progression = read_data<real_type>(input_dir, "cd4_progression", SS::hDS - 1, SS::hAG, SS::NS),
      .cd4_initial_distribution = read_data<real_type>(input_dir, "cd4_initial_distribution", SS::hDS, SS::hAG, SS::NS),
      .scale_cd4_mortality = read_data<int>(input_dir, "scale_cd4_mortality"),
      .idx_hm_elig = read_data<int>(input_dir, "idx_hm_elig", proj_years),
      .mortality = read_data<real_type>(input_dir, "mortality", SS::hTS, SS::hDS, SS::hAG, SS::NS),
      .mortality_time_rate_ratio = read_data<real_type>(input_dir, "mortality_time_rate_ratio", SS::hTS, proj_years),
      .dropout_recover_cd4 = read_data<int>(input_dir, "dropout_recover_cd4"),
      .dropout_rate = read_data<real_type>(input_dir, "dropout_rate", proj_years),
      .adults_on_art = read_data<real_type>(input_dir, "adults_on_art", SS::NS, proj_years),
      .adults_on_art_is_percent = read_data<int>(input_dir, "adults_on_art_is_percent", SS::NS, proj_years),
      .h_art_stage_dur = read_data<real_type>(input_dir, "h_art_stage_dur", SS::hTS - 1),
      .initiation_mortality_weight = read_data<real_type>(input_dir, "initiation_mortality_weight")
    };
  };

  static void write_output(
    std::filesystem::path& output_dir,
    const Config::OutputState& state
  ) {
    serialize::serialize_tensor<double, 3>(state.p_hiv_pop, output_dir / "p_hiv_pop");
    serialize::serialize_tensor<double, 3>(state.p_hiv_pop_natural_deaths, output_dir / "p_hiv_pop_natural_deaths");
    serialize::serialize_tensor<double, 4>(state.h_hiv_adult, output_dir / "h_hiv_adult");
    serialize::serialize_tensor<double, 5>(state.h_art_adult, output_dir / "h_art_adult");
    serialize::serialize_tensor<double, 4>(state.h_hiv_deaths_no_art, output_dir / "h_hiv_deaths_no_art");
    serialize::serialize_tensor<double, 3>(state.p_infections, output_dir / "p_infections");
    serialize::serialize_tensor<double, 5>(state.h_hiv_deaths_art, output_dir / "h_hiv_deaths_art");
    serialize::serialize_tensor<double, 4>(state.h_art_initiation, output_dir / "h_art_initiation");
    serialize::serialize_tensor<double, 3>(state.p_hiv_deaths, output_dir / "p_hiv_deaths");
  };
};


template<typename real_type, MV ModelVariant>
struct HcAdapterCpp {
  using SS = SSMixed<ModelVariant>;
  using Config = HcConfig<real_type, ModelVariant>;

  static Config::Pars get_pars(
    const std::filesystem::path &input_dir,
    const Opts<real_type> &opts,
    const int proj_years
  ) {
    return {
      .hc_nosocomial = read_data<real_type>(input_dir, "hc_nosocomial", proj_years),
      .hc1_cd4_dist = read_data<real_type>(input_dir, "hc1_cd4_dist", SS::hc2DS),
      .hc_cd4_transition = read_data<real_type>(input_dir, "hc_cd4_transition", SS::hc2DS, SS::hc1DS),
      .hc1_cd4_mort = read_data<real_type>(input_dir, "hc1_cd4_mort", SS::hc1DS, SS::hcTT, SS::hc1AG),
      .hc2_cd4_mort = read_data<real_type>(input_dir, "hc2_cd4_mort", SS::hc2DS, SS::hcTT, SS::hc2AG),
      .hc1_cd4_prog = read_data<real_type>(input_dir, "hc1_cd4_prog", SS::hc1DS, SS::hc1AG_c, SS::NS),
      .hc2_cd4_prog = read_data<real_type>(input_dir, "hc2_cd4_prog", SS::hc2DS, SS::hc2AG_c, SS::NS),
      .ctx_val = read_data<real_type>(input_dir, "ctx_val", proj_years),
      .hc_art_elig_age = read_data<int>(input_dir, "hc_art_elig_age", proj_years),
      .hc_art_elig_cd4 = read_data<real_type>(input_dir, "hc_art_elig_cd4", opts.p_idx_hiv_first_adult, proj_years),
      .hc_art_mort_rr = read_data<real_type>(input_dir, "hc_art_mort_rr", SS::hTS, opts.p_idx_hiv_first_adult, proj_years),
      .hc1_art_mort = read_data<real_type>(input_dir, "hc1_art_mort", SS::hc1DS, SS::hTS, SS::hc1AG),
      .hc2_art_mort = read_data<real_type>(input_dir, "hc2_art_mort", SS::hc2DS, SS::hTS, SS::hc2AG),
      .hc_art_isperc = read_data<int>(input_dir, "hc_art_isperc", proj_years),
      .hc_art_val = read_data<real_type>(input_dir, "hc_art_val", SS::hcAG_coarse, proj_years),
      .hc_art_init_dist = read_data<real_type>(input_dir, "hc_art_init_dist", opts.p_idx_hiv_first_adult, proj_years),
      .adult_cd4_dist = read_data<real_type>(input_dir, "adult_cd4_dist", SS::hDS, SS::hc2DS),
      .fert_mult_by_age = read_data<real_type>(input_dir, "fert_mult_by_age", opts.p_fertility_age_groups),
      .fert_mult_off_art = read_data<real_type>(input_dir, "fert_mult_off_art", SS::hDS),
      .fert_mult_on_art = read_data<real_type>(input_dir, "fert_mult_on_art", opts.p_fertility_age_groups),
      .total_fertility_rate = read_data<real_type>(input_dir, "total_fertility_rate", proj_years),
      .PMTCT = read_data<real_type>(input_dir, "PMTCT", SS::hPS, proj_years),
      .vertical_transmission_rate = read_data<real_type>(input_dir, "vertical_transmission_rate", SS::hDS + 1, SS::hVT),
      .PMTCT_transmission_rate = read_data<real_type>(input_dir, "PMTCT_transmission_rate", SS::hDS, SS::hPS, SS::hVT),
      .PMTCT_dropout = read_data<real_type>(input_dir, "PMTCT_dropout", SS::hPS_dropout, proj_years),
      .PMTCT_input_is_percent = read_data<int>(input_dir, "PMTCT_input_is_percent", proj_years),
      .breastfeeding_duration_art = read_data<real_type>(input_dir, "breastfeeding_duration_art", SS::hBF, proj_years),
      .breastfeeding_duration_no_art = read_data<real_type>(input_dir, "breastfeeding_duration_no_art", SS::hBF, proj_years),
      .mat_hiv_births = read_data<real_type>(input_dir, "mat_hiv_births", proj_years),
      .mat_prev_input = read_data<int>(input_dir, "mat_prev_input", proj_years),
      .prop_lt200 = read_data<real_type>(input_dir, "prop_lt200", proj_years),
      .prop_gte350 = read_data<real_type>(input_dir, "prop_gte350", proj_years),
      .incrate = read_data<real_type>(input_dir, "incrate", proj_years),
      .ctx_val_is_percent = read_data<int>(input_dir, "ctx_val_is_percent", proj_years),
      .hc_art_is_age_spec = read_data<int>(input_dir, "hc_art_is_age_spec", proj_years),
      .hc_age_coarse = read_data<real_type>(input_dir, "hc_age_coarse", SS::hcAG_end),
      .abortion = read_data<real_type>(input_dir, "abortion", SS::hAB_ind, proj_years),
      .patients_reallocated = read_data<real_type>(input_dir, "patients_reallocated", proj_years),
      .hc_art_ltfu = read_data<real_type>(input_dir, "hc_art_ltfu", proj_years),
      .hc_age_coarse_cd4 = read_data<int>(input_dir, "hc_age_coarse_cd4", opts.p_idx_hiv_first_adult),
      .adult_female_infections = read_data<real_type>(input_dir, "adult_female_infections", opts.p_fertility_age_groups, proj_years),
      .adult_female_hivnpop = read_data<real_type>(input_dir, "adult_female_hivnpop", opts.p_fertility_age_groups, proj_years),
      .total_births = read_data<real_type>(input_dir, "total_births", proj_years),
      .ctx_effect = read_data<real_type>(input_dir, "ctx_effect", 3),
      .hc_art_start = read_data<real_type>(input_dir, "hc_art_start"),
      .local_adj_factor = read_data<real_type>(input_dir, "local_adj_factor")
    };
  };

  static void write_output(
    std::filesystem::path& output_dir,
    const Config::OutputState& state
  ) {
    serialize::serialize_tensor<double, 5>(state.hc1_hiv_pop, output_dir / "hc1_hiv_pop");
    serialize::serialize_tensor<double, 5>(state.hc2_hiv_pop, output_dir / "hc2_hiv_pop");
    serialize::serialize_tensor<double, 5>(state.hc1_art_pop, output_dir / "hc1_art_pop");
    serialize::serialize_tensor<double, 5>(state.hc2_art_pop, output_dir / "hc2_art_pop");
    serialize::serialize_tensor<double, 5>(state.hc1_noart_aids_deaths, output_dir / "hc1_noart_aids_deaths");
    serialize::serialize_tensor<double, 5>(state.hc2_noart_aids_deaths, output_dir / "hc2_noart_aids_deaths");
    serialize::serialize_tensor<double, 5>(state.hc1_art_aids_deaths, output_dir / "hc1_art_aids_deaths");
    serialize::serialize_tensor<double, 5>(state.hc2_art_aids_deaths, output_dir / "hc2_art_aids_deaths");
    serialize::serialize_tensor<double, 2>(state.hc_art_init, output_dir / "hc_art_init");
    serialize::serialize_tensor<double, 5>(state.hc_art_need_init, output_dir / "hc_art_need_init");
    serialize::serialize_tensor<double, 1>(state.hiv_births, output_dir / "hiv_births");
    serialize::serialize_tensor<double, 1>(state.ctx_need, output_dir / "ctx_need");
    serialize::serialize_tensor<double, 4>(state.infection_by_type, output_dir / "infection_by_type");
  };
};


}
}

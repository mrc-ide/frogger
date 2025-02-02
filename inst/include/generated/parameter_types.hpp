// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `parameter_types.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include "../intermediate_data.hpp"
#include "../state_space.hpp"
#include "../model_variants.hpp"

namespace leapfrog {

template<typename real_type>
struct Demography {
  TensorMap2<real_type> base_pop;
  TensorMap3<real_type> survival_probability;
  TensorMap3<real_type> net_migration;
  TensorMap2<real_type> age_specific_fertility_rate;
  TensorMap2<real_type> births_sex_prop;
};

template<typename real_type>
struct Incidence {
  TensorMap1<real_type> total_rate;
  TensorMap3<real_type> relative_risk_age;
  TensorMap1<real_type> relative_risk_sex;
};

template<typename real_type>
struct NaturalHistory {
  TensorMap3<real_type> cd4_mortality;
  TensorMap3<real_type> cd4_progression;
  TensorMap3<real_type> cd4_initial_distribution;
  int scale_cd4_mortality;
};

template<typename real_type>
struct Art {
  Tensor1<int> idx_hm_elig;
  TensorMap4<real_type> mortality;
  TensorMap2<real_type> mortality_time_rate_ratio;
  int dropout_recover_cd4;
  TensorMap1<real_type> dropout_rate;
  TensorMap2<real_type> adults_on_art;
  TensorMap2<int> adults_on_art_is_percent;
  Tensor1<real_type> h_art_stage_dur;
  real_type initiation_mortality_weight;
};

template<typename real_type>
struct Children {
  TensorMap1<real_type> hc_nosocomial;
  TensorMap1<real_type> hc1_cd4_dist;
  TensorMap2<real_type> hc_cd4_transition;
  TensorMap3<real_type> hc1_cd4_mort;
  TensorMap3<real_type> hc2_cd4_mort;
  TensorMap3<real_type> hc1_cd4_prog;
  TensorMap3<real_type> hc2_cd4_prog;
  TensorMap1<real_type> ctx_effect;
  TensorMap1<real_type> ctx_val;
  TensorMap1<int> hc_art_elig_age;
  Tensor2<real_type> hc_art_elig_cd4;
  TensorMap3<real_type> hc_art_mort_rr;
  TensorMap3<real_type> hc1_art_mort;
  TensorMap3<real_type> hc2_art_mort;
  TensorMap1<int> hc_art_isperc;
  TensorMap2<real_type> hc_art_val;
  TensorMap2<real_type> hc_art_init_dist;
  TensorMap2<real_type> adult_cd4_dist;
  TensorMap1<real_type> fert_mult_by_age;
  TensorMap1<real_type> fert_mult_off_art;
  TensorMap1<real_type> fert_mult_on_art;
  TensorMap1<real_type> total_fertility_rate;
  real_type local_adj_factor;
  TensorMap2<real_type> PMTCT;
  TensorMap2<real_type> vertical_transmission_rate;
  TensorMap3<real_type> PMTCT_transmission_rate;
  TensorMap2<real_type> PMTCT_dropout;
  TensorMap1<int> PMTCT_input_is_percent;
  TensorMap2<real_type> breastfeeding_duration_art;
  TensorMap2<real_type> breastfeeding_duration_no_art;
  TensorMap1<real_type> mat_hiv_births;
  TensorMap1<int> mat_prev_input;
  TensorMap1<real_type> prop_lt200;
  TensorMap1<real_type> prop_gte350;
  TensorMap1<real_type> incrate;
  TensorMap1<int> ctx_val_is_percent;
  TensorMap1<int> hc_art_is_age_spec;
  TensorMap1<real_type> hc_age_coarse;
  TensorMap2<real_type> abortion;
  TensorMap1<real_type> patients_reallocated;
  TensorMap1<real_type> hc_art_ltfu;
  TensorMap1<int> hc_age_coarse_cd4;
  TensorMap2<real_type> adult_female_infections;
  TensorMap2<real_type> adult_female_hivnpop;
  TensorMap1<real_type> total_births;
};

}

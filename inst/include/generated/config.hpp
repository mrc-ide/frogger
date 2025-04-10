// Generated by cpp_generation: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit json files in `cpp_generation/modelSchemas` and run `uv run ./src/main.py` inside
// the `cpp_generation` folder.

#pragma once

#include "state_space_mixer.hpp"
#include "tensor_types.hpp"

namespace leapfrog {
namespace internal {

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
    TM1<real_type> ctx_effect;
    real_type hc_art_start;
    real_type local_adj_factor;
  };

  struct Intermediate {
    TFS<real_type, SS::hDS, SS::NS> age15_hiv_pop;
    TFS<real_type, SS::hTS, SS::hDS, SS::NS> age15_art_pop;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hAG, SS::NS> hc_posthivmort;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hAG, SS::NS> hc_grad;
    TFS<real_type, SS::hDS, SS::hAG, SS::NS> eligible;
    TFS<real_type, SS::hcAG_coarse> unmet_need;
    TFS<real_type, SS::hcAG_coarse> total_need;
    TFS<real_type, SS::hcAG_coarse> on_art;
    TFS<real_type, SS::hcAG_coarse> total_art_last_year;
    TFS<real_type, SS::hcAG_coarse> total_art_this_year;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hAG, SS::NS> hc_art_grad;
    TFS<real_type, SS::hcAG_coarse> hc_art_scalar;
    TFS<real_type, SS::hcAG_coarse> hc_initByAge;
    TFS<real_type, SS::hcAG_coarse> hc_adj;
    TFS<real_type, SS::hcAG_coarse> hc_art_deaths;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hcAG_end, SS::NS> hc_hiv_dist;
    TFS<real_type, SS::hDS,  SS::hcAG_end, SS::NS> hc_hiv_total;
    TFS<real_type, SS::hDS, SS::hcTT, SS::hcAG_end, SS::NS> art_ltfu_grad;
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
    real_type on_PMTCT;
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
    real_type bf_scalar;
    TFS<real_type, 3> ctx_mean;

    Intermediate() {};

    void reset() {
      age15_hiv_pop.setZero();
      age15_art_pop.setZero();
      hc_posthivmort.setZero();
      hc_grad.setZero();
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
      on_PMTCT = 0;
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
      bf_scalar = 0;
      ctx_mean.setZero();
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
    TFS<real_type, SS::hcTT, SS::hc1AG, SS::NS> infection_by_type;

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
      infection_by_type(initial_state.infection_by_type)
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
      infection_by_type.setZero();
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
    T4<real_type> infection_by_type;

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
      infection_by_type(SS::hcTT, SS::hc1AG, SS::NS, output_years)
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
      infection_by_type.setZero();
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
      infection_by_type.chip(i, infection_by_type.NumDimensions - 1) = state.infection_by_type;
    };
  };

  static constexpr int output_count = 13;
  static int get_build_output_size(int prev_size) {
    return prev_size + output_count;
  };
};


} // namespace internal
} // namespace leapfrog

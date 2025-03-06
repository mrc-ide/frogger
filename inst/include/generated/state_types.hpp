// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `state_types.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include "../intermediate_data.hpp"
#include "../parameters.hpp"
#include "../state_space.hpp"
#include "../model_variants.hpp"

namespace leapfrog {

namespace {
using Eigen::Sizes;
using Eigen::TensorFixedSize;
}

template<typename ModelVariant, typename real_type>
struct ChildModelState {
  ChildModelState() {}

  void reset() {}
};

template<typename ModelVariant, typename real_type>
struct DemographicProjectionState {
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_total_pop;
  real_type births;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_total_pop_natural_deaths;

  // Used by R
  DemographicProjectionState() {
    reset();
  }

  // Used by Python
  DemographicProjectionState(
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_total_pop,
    real_type births,
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_total_pop_natural_deaths
  ) :
    p_total_pop(p_total_pop),
    births(births),
    p_total_pop_natural_deaths(p_total_pop_natural_deaths) {}

  void reset() {
    p_total_pop.setZero();
    births = 0;
    p_total_pop_natural_deaths.setZero();
  }
};

template<typename ModelVariant, typename real_type, typename Enable = void>
struct HivSimulationState {
  HivSimulationState() {}

  void reset() {}
};

template<typename ModelVariant, typename real_type>
struct HivSimulationState<ModelVariant, real_type,
    std::enable_if_t<ModelVariant::run_hiv_simulation>> {
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_pop;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_pop_natural_deaths;
  TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_adult;
  TensorFixedSize<real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_art_adult;
  TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_deaths_no_art;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_infections;
  TensorFixedSize<real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_deaths_art;
  TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_art_initiation;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_deaths;

  // Used by R
  HivSimulationState() {
    reset();
  }

  // Used by Python
  HivSimulationState(
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_hiv_pop,
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_hiv_pop_natural_deaths,
    const TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>>& h_hiv_adult,
    const TensorFixedSize<real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>>& h_art_adult,
    const TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>>& h_hiv_deaths_no_art,
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_infections,
    const TensorFixedSize<real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>>& h_hiv_deaths_art,
    const TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>>& h_art_initiation,
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_hiv_deaths
  ) :
    p_hiv_pop(p_hiv_pop),
    p_hiv_pop_natural_deaths(p_hiv_pop_natural_deaths),
    h_hiv_adult(h_hiv_adult),
    h_art_adult(h_art_adult),
    h_hiv_deaths_no_art(h_hiv_deaths_no_art),
    p_infections(p_infections),
    h_hiv_deaths_art(h_hiv_deaths_art),
    h_art_initiation(h_art_initiation),
    p_hiv_deaths(p_hiv_deaths) {}

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
  }
};

template<typename real_type>
struct ChildModelState<ChildModel, real_type> {
  TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_hiv_pop;
  TensorFixedSize<real_type, Sizes<hc2DS<ChildModel>, hcTT<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_hiv_pop;
  TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc1DS<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_art_pop;
  TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc2DS<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_art_pop;
  TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_noart_aids_deaths;
  TensorFixedSize<real_type, Sizes<hc2DS<ChildModel>, hcTT<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_noart_aids_deaths;
  TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc1DS<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_art_aids_deaths;
  TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc2DS<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_art_aids_deaths;
  real_type hiv_births;
  TensorFixedSize<real_type, Sizes<hcAG_coarse<ChildModel>>> hc_art_init;
  TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hcAG_end<ChildModel>, NS<ChildModel>>> hc_art_need_init;
  real_type ctx_need;
  TensorFixedSize<real_type, Sizes<hcTT<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> infection_by_type;
  TensorFixedSize<real_type, Sizes<2>> ctx_mean;

  // Used by R
  ChildModelState() {
    reset();
  }

  // Used by Python
  ChildModelState(
    const TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>>& hc1_hiv_pop,
    const TensorFixedSize<real_type, Sizes<hc2DS<ChildModel>, hcTT<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>>& hc2_hiv_pop,
    const TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc1DS<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>>& hc1_art_pop,
    const TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc2DS<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>>& hc2_art_pop,
    const TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>>& hc1_noart_aids_deaths,
    const TensorFixedSize<real_type, Sizes<hc2DS<ChildModel>, hcTT<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>>& hc2_noart_aids_deaths,
    const TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc1DS<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>>& hc1_art_aids_deaths,
    const TensorFixedSize<real_type, Sizes<hTS<ChildModel>, hc2DS<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>>& hc2_art_aids_deaths,
    const real_type hiv_births,
    const TensorFixedSize<real_type, Sizes<hcAG_coarse<ChildModel>>>& hc_art_init,
    const TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hcAG_end<ChildModel>, NS<ChildModel>>>& hc_art_need_init,
    const real_type ctx_need,
    const real_type ctx_mean
  ) :
    hc1_hiv_pop(hc1_hiv_pop),
    hc2_hiv_pop(hc2_hiv_pop),
    hc1_art_pop(hc1_art_pop),
    hc2_art_pop(hc2_art_pop),
    hc1_noart_aids_deaths(hc1_noart_aids_deaths),
    hc2_noart_aids_deaths(hc2_noart_aids_deaths),
    hc1_art_aids_deaths(hc1_art_aids_deaths),
    hc2_art_aids_deaths(hc2_art_aids_deaths),
    hiv_births(hiv_births),
    hc_art_init(hc_art_init),
    hc_art_need_init(hc_art_need_init),
    ctx_need(ctx_need),
    ctx_mean(ctx_mean) {}

  void reset() {
    hc1_hiv_pop.setZero();
    hc2_hiv_pop.setZero();
    hc1_art_pop.setZero();
    hc2_art_pop.setZero();
    hc1_noart_aids_deaths.setZero();
    hc2_noart_aids_deaths.setZero();
    hc1_art_aids_deaths.setZero();
    hc2_art_aids_deaths.setZero();
    hiv_births = 0;
    hc_art_init.setZero();
    hc_art_need_init.setZero();
    ctx_need = 0;
    infection_by_type.setZero();
    ctx_mean.setZero();
  }
};

template<typename ModelVariant, typename real_type>
struct State {
  DemographicProjectionState<ModelVariant, real_type> dp;
  HivSimulationState<ModelVariant, real_type> hiv;
  ChildModelState<ModelVariant, real_type> children;

  State() :
      dp(),
      hiv(),
      children() {}

  State(const DemographicProjectionState<ModelVariant, real_type>& dp,
        const HivSimulationState<ModelVariant, real_type>& hiv,
        const ChildModelState<ModelVariant, real_type>& children) :
      dp(dp),
      hiv(hiv),
      children(children) {}

  void reset() {
    dp.reset();
    hiv.reset();
    children.reset();
  }
};

}

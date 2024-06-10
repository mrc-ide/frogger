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
  ChildModelState(const Parameters<ModelVariant, real_type> &pars) {}

  void reset() {}
};

template<typename ModelVariant, typename real_type>
struct BaseModelState {
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_total_pop;
  real_type births;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_total_pop_natural_deaths;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_pop;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_pop_natural_deaths;
  TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_adult;
  TensorFixedSize<real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_art_adult;
  TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_deaths_no_art;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_infections;
  TensorFixedSize<real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_deaths_art;
  TensorFixedSize<real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_art_initiation;
  TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_deaths;

  BaseModelState(const Parameters<ModelVariant, real_type> &pars) {
    reset();
  }

  BaseModelState(
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_total_pop,
    real_type births,
    const TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>>& p_total_pop_natural_deaths,
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
    p_total_pop(p_total_pop),
    births(births),
    p_total_pop_natural_deaths(p_total_pop_natural_deaths),
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
    p_total_pop.setZero();
    births = 0;
    p_total_pop_natural_deaths.setZero();
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
  TensorFixedSize<real_type, Sizes<4>> hc_art_total;
  TensorFixedSize<real_type, Sizes<4>> hc_art_init;
  TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, 15, NS<ChildModel>>> hc_art_need_init;
  real_type ctx_need;
  real_type ctx_mean;

  ChildModelState(const Parameters<ChildModel, real_type> &pars) {
    reset();
  }

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
    hc_art_total.setZero();
    hc_art_init.setZero();
    hc_art_need_init.setZero();
    ctx_need = 0;
    ctx_mean = 0;
  }
};

template<typename ModelVariant, typename real_type>
struct State {
  BaseModelState<ModelVariant, real_type> base;
  ChildModelState<ModelVariant, real_type> children;

  State(const Parameters<ModelVariant, real_type> &pars) :
      base(pars),
      children(pars) {}

  State(const BaseModelState<ModelVariant, real_type> &base,
        const ChildModelState<ModelVariant, real_type> &children)
    : base(base),
      children(children) {}

  void reset() {
    base.reset();
    children.reset();
  }
};

}

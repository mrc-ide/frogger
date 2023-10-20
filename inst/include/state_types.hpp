#pragma once

#include "types.hpp"
#include "parameters.hpp"
#include "state_space.hpp"
#include "model_variants.hpp"

namespace leapfrog {

namespace {
using Eigen::Sizes;
using Eigen::TensorFixedSize;
}

template<typename ModelVariant, typename real_type>
struct BaseModelState {
  TensorFixedSize <real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_total_pop;
  TensorFixedSize <real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_total_pop_natural_deaths;
  TensorFixedSize <real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_pop;
  TensorFixedSize <real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_pop_natural_deaths;
  TensorFixedSize <real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_adult;
  TensorFixedSize <real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>>
      h_art_adult;
  real_type births;
  TensorFixedSize <real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_hiv_deaths_no_art;
  TensorFixedSize <real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_infections;
  TensorFixedSize <real_type, Sizes<hTS<ModelVariant>, hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>>
      h_hiv_deaths_art;
  TensorFixedSize <real_type, Sizes<hDS<ModelVariant>, hAG<ModelVariant>, NS<ModelVariant>>> h_art_initiation;
  TensorFixedSize <real_type, Sizes<pAG<ModelVariant>, NS<ModelVariant>>> p_hiv_deaths;

  BaseModelState(const Parameters<ModelVariant, real_type> &pars) {
    reset();
  }

  void reset() {
    p_total_pop.setZero();
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
    births = 0;
  }
};

template<typename ModelVariant, typename real_type>
struct ChildModelState {
  ChildModelState(const Parameters<ModelVariant, real_type> &pars) {}

  void reset() {}
};

template<typename real_type>
struct ChildModelState<ChildModel, real_type> {
  TensorFixedSize <real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_hiv_pop;
  TensorFixedSize <real_type, Sizes<hc2DS<ChildModel>, hcTT<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_hiv_pop;
  TensorFixedSize <real_type, Sizes<hTS<ChildModel>, hc1DS<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_art_pop;
  TensorFixedSize <real_type, Sizes<hTS<ChildModel>, hc2DS<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_art_pop;
  TensorFixedSize <real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_noart_aids_deaths;
  TensorFixedSize <real_type, Sizes<hc2DS<ChildModel>, hcTT<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_noart_aids_deaths;
  TensorFixedSize <real_type, Sizes<hTS<ChildModel>, hc1DS<ChildModel>, hc1AG<ChildModel>, NS<ChildModel>>> hc1_art_aids_deaths;
  TensorFixedSize <real_type, Sizes<hTS<ChildModel>, hc2DS<ChildModel>, hc2AG<ChildModel>, NS<ChildModel>>> hc2_art_aids_deaths;

  real_type hc_art_num;

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
    hc_art_num = 0.0;
  }
};

template<typename ModelVariant, typename real_type>
struct State {
  BaseModelState<ModelVariant, real_type> base;
  ChildModelState<ModelVariant, real_type> children;

  State(const Parameters<ModelVariant, real_type> &pars) :
      base(pars),
      children(pars) {}

  void reset() {
    base.reset();
    children.reset();
  }
};

}

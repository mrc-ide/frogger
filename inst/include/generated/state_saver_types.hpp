// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `state_saver_types.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include "../intermediate_data.hpp"
#include "../model_variants.hpp"

namespace leapfrog {

template<typename ModelVariant, typename real_type>
struct ChildModelOutputState {
  ChildModelOutputState(int output_years) {}
};

template<typename ModelVariant, typename real_type>
struct DemographicProjectionOutputState {
  Tensor3<real_type> p_total_pop;
  Tensor1<real_type> births;
  Tensor3<real_type> p_total_pop_natural_deaths;

  DemographicProjectionOutputState(int output_years):
    p_total_pop(
      StateSpace<ModelVariant>().dp.pAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    births(
      output_years
    ),
    p_total_pop_natural_deaths(
      StateSpace<ModelVariant>().dp.pAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ) {
    p_total_pop.setZero();
    births.setZero();
    p_total_pop_natural_deaths.setZero();
  }
};

template<typename ModelVariant, typename real_type, typename Enable = void>
struct HivSimulationOutputState {
  HivSimulationOutputState(int output_years) {}
};

template<typename ModelVariant, typename real_type>
struct HivSimulationOutputState<ModelVariant, real_type, std::enable_if_t<ModelVariant::run_hiv_simulation>> {
  Tensor3<real_type> p_hiv_pop;
  Tensor3<real_type> p_hiv_pop_natural_deaths;
  Tensor4<real_type> h_hiv_adult;
  Tensor5<real_type> h_art_adult;
  Tensor4<real_type> h_hiv_deaths_no_art;
  Tensor3<real_type> p_infections;
  Tensor5<real_type> h_hiv_deaths_art;
  Tensor4<real_type> h_art_initiation;
  Tensor3<real_type> p_hiv_deaths;

  HivSimulationOutputState(int output_years):
    p_hiv_pop(
      StateSpace<ModelVariant>().dp.pAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    p_hiv_pop_natural_deaths(
      StateSpace<ModelVariant>().dp.pAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    h_hiv_adult(
      StateSpace<ModelVariant>().hiv.hDS,
      StateSpace<ModelVariant>().hiv.hAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    h_art_adult(
      StateSpace<ModelVariant>().hiv.hTS,
      StateSpace<ModelVariant>().hiv.hDS,
      StateSpace<ModelVariant>().hiv.hAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    h_hiv_deaths_no_art(
      StateSpace<ModelVariant>().hiv.hDS,
      StateSpace<ModelVariant>().hiv.hAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    p_infections(
      StateSpace<ModelVariant>().dp.pAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    h_hiv_deaths_art(
      StateSpace<ModelVariant>().hiv.hTS,
      StateSpace<ModelVariant>().hiv.hDS,
      StateSpace<ModelVariant>().hiv.hAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    h_art_initiation(
      StateSpace<ModelVariant>().hiv.hDS,
      StateSpace<ModelVariant>().hiv.hAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ),
    p_hiv_deaths(
      StateSpace<ModelVariant>().dp.pAG,
      StateSpace<ModelVariant>().dp.NS,
      output_years
    ) {
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
struct ChildModelOutputState<ChildModel, real_type> {
  Tensor5<real_type> hc1_hiv_pop;
  Tensor5<real_type> hc2_hiv_pop;
  Tensor5<real_type> hc1_art_pop;
  Tensor5<real_type> hc2_art_pop;
  Tensor5<real_type> hc1_noart_aids_deaths;
  Tensor5<real_type> hc2_noart_aids_deaths;
  Tensor5<real_type> hc1_art_aids_deaths;
  Tensor5<real_type> hc2_art_aids_deaths;
  Tensor1<real_type> hiv_births;
  Tensor2<real_type> hc_art_init;
  Tensor5<real_type> hc_art_need_init;
  Tensor1<real_type> ctx_need;
  Tensor1<real_type> ctx_mean;

  ChildModelOutputState(int output_years):
    hc1_hiv_pop(
      StateSpace<ChildModel>().children.hc1DS,
      StateSpace<ChildModel>().children.hcTT,
      StateSpace<ChildModel>().children.hc1AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hc2_hiv_pop(
      StateSpace<ChildModel>().children.hc2DS,
      StateSpace<ChildModel>().children.hcTT,
      StateSpace<ChildModel>().children.hc2AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hc1_art_pop(
      StateSpace<ChildModel>().hiv.hTS,
      StateSpace<ChildModel>().children.hc1DS,
      StateSpace<ChildModel>().children.hc1AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hc2_art_pop(
      StateSpace<ChildModel>().hiv.hTS,
      StateSpace<ChildModel>().children.hc2DS,
      StateSpace<ChildModel>().children.hc2AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hc1_noart_aids_deaths(
      StateSpace<ChildModel>().children.hc1DS,
      StateSpace<ChildModel>().children.hcTT,
      StateSpace<ChildModel>().children.hc1AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hc2_noart_aids_deaths(
      StateSpace<ChildModel>().children.hc2DS,
      StateSpace<ChildModel>().children.hcTT,
      StateSpace<ChildModel>().children.hc2AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hc1_art_aids_deaths(
      StateSpace<ChildModel>().hiv.hTS,
      StateSpace<ChildModel>().children.hc1DS,
      StateSpace<ChildModel>().children.hc1AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hc2_art_aids_deaths(
      StateSpace<ChildModel>().hiv.hTS,
      StateSpace<ChildModel>().children.hc2DS,
      StateSpace<ChildModel>().children.hc2AG,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    hiv_births(
      output_years
    ),
    hc_art_init(
      StateSpace<ChildModel>().children.hcAG_coarse,
      output_years
    ),
    hc_art_need_init(
      StateSpace<ChildModel>().children.hc1DS,
      StateSpace<ChildModel>().children.hcTT,
      StateSpace<ChildModel>().children.hcAG_end,
      StateSpace<ChildModel>().dp.NS,
      output_years
    ),
    ctx_need(
      output_years
    ),
    ctx_mean(
      output_years
    ) {
    hc1_hiv_pop.setZero();
    hc2_hiv_pop.setZero();
    hc1_art_pop.setZero();
    hc2_art_pop.setZero();
    hc1_noart_aids_deaths.setZero();
    hc2_noart_aids_deaths.setZero();
    hc1_art_aids_deaths.setZero();
    hc2_art_aids_deaths.setZero();
    hiv_births.setZero();
    hc_art_init.setZero();
    hc_art_need_init.setZero();
    ctx_need.setZero();
    ctx_mean.setZero();
  }
};


template<typename ModelVariant, typename real_type>
struct ChildModelStateSaver {
public:
  void save_state(ChildModelOutputState<ModelVariant, real_type> &full_state,
                  const size_t i,
                  const State<ModelVariant, real_type> &state) {}
};

template<typename ModelVariant, typename real_type>
struct DemographicProjectionStateSaver {
public:
  void save_state(DemographicProjectionOutputState<ModelVariant, real_type> &output_state,
                  const size_t i,
                  const State<ModelVariant, real_type> &state) {
    output_state.p_total_pop.chip(i, output_state.p_total_pop.NumDimensions - 1) = state.dp.p_total_pop;
    output_state.births(i) = state.dp.births;
    output_state.p_total_pop_natural_deaths.chip(i, output_state.p_total_pop_natural_deaths.NumDimensions - 1) = state.dp.p_total_pop_natural_deaths;
    return;
  }
};

template<typename ModelVariant, typename real_type, typename Enable = void>
struct HivSimulationStateSaver {
public:
  void save_state(HivSimulationOutputState<ModelVariant, real_type> &full_state,
                  const size_t i,
                  const State<ModelVariant, real_type> &state) {}
};

template<typename ModelVariant, typename real_type>
struct HivSimulationStateSaver<ModelVariant, real_type, std::enable_if_t<ModelVariant::run_hiv_simulation>> {
public:
  void save_state(HivSimulationOutputState<ModelVariant, real_type> &output_state,
                  const size_t i,
                  const State<ModelVariant, real_type> &state) {
    output_state.p_hiv_pop.chip(i, output_state.p_hiv_pop.NumDimensions - 1) = state.hiv.p_hiv_pop;
    output_state.p_hiv_pop_natural_deaths.chip(i, output_state.p_hiv_pop_natural_deaths.NumDimensions - 1) = state.hiv.p_hiv_pop_natural_deaths;
    output_state.h_hiv_adult.chip(i, output_state.h_hiv_adult.NumDimensions - 1) = state.hiv.h_hiv_adult;
    output_state.h_art_adult.chip(i, output_state.h_art_adult.NumDimensions - 1) = state.hiv.h_art_adult;
    output_state.h_hiv_deaths_no_art.chip(i, output_state.h_hiv_deaths_no_art.NumDimensions - 1) = state.hiv.h_hiv_deaths_no_art;
    output_state.p_infections.chip(i, output_state.p_infections.NumDimensions - 1) = state.hiv.p_infections;
    output_state.h_hiv_deaths_art.chip(i, output_state.h_hiv_deaths_art.NumDimensions - 1) = state.hiv.h_hiv_deaths_art;
    output_state.h_art_initiation.chip(i, output_state.h_art_initiation.NumDimensions - 1) = state.hiv.h_art_initiation;
    output_state.p_hiv_deaths.chip(i, output_state.p_hiv_deaths.NumDimensions - 1) = state.hiv.p_hiv_deaths;
    return;
  }
};

template<typename real_type>
struct ChildModelStateSaver<ChildModel, real_type> {
public:
  void save_state(ChildModelOutputState<ChildModel, real_type> &output_state,
                  const size_t i,
                  const State<ChildModel, real_type> &state) {
    output_state.hc1_hiv_pop.chip(i, output_state.hc1_hiv_pop.NumDimensions - 1) = state.children.hc1_hiv_pop;
    output_state.hc2_hiv_pop.chip(i, output_state.hc2_hiv_pop.NumDimensions - 1) = state.children.hc2_hiv_pop;
    output_state.hc1_art_pop.chip(i, output_state.hc1_art_pop.NumDimensions - 1) = state.children.hc1_art_pop;
    output_state.hc2_art_pop.chip(i, output_state.hc2_art_pop.NumDimensions - 1) = state.children.hc2_art_pop;
    output_state.hc1_noart_aids_deaths.chip(i, output_state.hc1_noart_aids_deaths.NumDimensions - 1) = state.children.hc1_noart_aids_deaths;
    output_state.hc2_noart_aids_deaths.chip(i, output_state.hc2_noart_aids_deaths.NumDimensions - 1) = state.children.hc2_noart_aids_deaths;
    output_state.hc1_art_aids_deaths.chip(i, output_state.hc1_art_aids_deaths.NumDimensions - 1) = state.children.hc1_art_aids_deaths;
    output_state.hc2_art_aids_deaths.chip(i, output_state.hc2_art_aids_deaths.NumDimensions - 1) = state.children.hc2_art_aids_deaths;
    output_state.hiv_births(i) = state.children.hiv_births;
    output_state.hc_art_init.chip(i, output_state.hc_art_init.NumDimensions - 1) = state.children.hc_art_init;
    output_state.hc_art_need_init.chip(i, output_state.hc_art_need_init.NumDimensions - 1) = state.children.hc_art_need_init;
    output_state.ctx_need(i) = state.children.ctx_need;
    output_state.ctx_mean(i) = state.children.ctx_mean;
    return;
  }
};

}

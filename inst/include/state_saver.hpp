#pragma once

#include "types.hpp"
#include "model_variants.hpp"

namespace leapfrog {

template<typename ModelVariant, typename real_type>
struct BaseModelOutputState {
  Tensor3<real_type> p_total_pop;
  Tensor3<real_type> p_total_pop_natural_deaths;
  Tensor3<real_type> p_hiv_pop;
  Tensor3<real_type> p_hiv_pop_natural_deaths;
  Tensor4<real_type> h_hiv_adult;
  Tensor5<real_type> h_art_adult;
  Tensor1<real_type> births;
  Tensor4<real_type> h_hiv_deaths_no_art;
  Tensor3<real_type> p_infections;
  Tensor5<real_type> h_hiv_deaths_art;
  Tensor4<real_type> h_art_initiation;
  Tensor3<real_type> p_hiv_deaths;

  BaseModelOutputState(int no_output_years)
      : p_total_pop(StateSpace<ModelVariant>().base.pAG,
                    StateSpace<ModelVariant>().base.NS, no_output_years),
        p_total_pop_natural_deaths(StateSpace<ModelVariant>().base.pAG,
                                   StateSpace<ModelVariant>().base.NS, no_output_years),
        p_hiv_pop(StateSpace<ModelVariant>().base.pAG,
                  StateSpace<ModelVariant>().base.NS, no_output_years),
        p_hiv_pop_natural_deaths(StateSpace<ModelVariant>().base.pAG,
                                 StateSpace<ModelVariant>().base.NS, no_output_years),
        h_hiv_adult(StateSpace<ModelVariant>().base.hDS,
                    StateSpace<ModelVariant>().base.hAG,
                    StateSpace<ModelVariant>().base.NS,
                    no_output_years),
        h_art_adult(StateSpace<ModelVariant>().base.hTS,
                    StateSpace<ModelVariant>().base.hDS,
                    StateSpace<ModelVariant>().base.hAG,
                    StateSpace<ModelVariant>().base.NS,
                    no_output_years),
        births(no_output_years),
        h_hiv_deaths_no_art(StateSpace<ModelVariant>().base.hDS,
                            StateSpace<ModelVariant>().base.hAG,
                            StateSpace<ModelVariant>().base.NS, no_output_years),
        p_infections(StateSpace<ModelVariant>().base.pAG,
                     StateSpace<ModelVariant>().base.NS, no_output_years),
        h_hiv_deaths_art(StateSpace<ModelVariant>().base.hTS,
                         StateSpace<ModelVariant>().base.hDS,
                         StateSpace<ModelVariant>().base.hAG,
                         StateSpace<ModelVariant>().base.NS, no_output_years),
        h_art_initiation(StateSpace<ModelVariant>().base.hDS,
                         StateSpace<ModelVariant>().base.hAG,
                         StateSpace<ModelVariant>().base.NS, no_output_years),
        p_hiv_deaths(StateSpace<ModelVariant>().base.pAG,
                     StateSpace<ModelVariant>().base.NS, no_output_years) {
    p_total_pop.setZero();
    p_total_pop_natural_deaths.setZero();
    p_hiv_pop.setZero();
    p_hiv_pop_natural_deaths.setZero();
    h_hiv_adult.setZero();
    h_art_adult.setZero();
    births.setZero();
    h_hiv_deaths_no_art.setZero();
    p_infections.setZero();
    h_hiv_deaths_art.setZero();
    h_art_initiation.setZero();
    p_hiv_deaths.setZero();
  }
};

template<typename ModelVariant, typename real_type>
struct ChildModelOutputState {
  ChildModelOutputState(int no_output_years) {}
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
  Tensor1<real_type> hc_art_num;


  ChildModelOutputState(int no_output_years)
      : hc1_hiv_pop(StateSpace<ChildModel>().children.hc1DS,
                    StateSpace<ChildModel>().children.hcTT,
                    StateSpace<ChildModel>().children.hc1AG,
                    StateSpace<ChildModel>().base.NS, no_output_years),
        hc2_hiv_pop(StateSpace<ChildModel>().children.hc2DS,
                    StateSpace<ChildModel>().children.hcTT,
                    StateSpace<ChildModel>().children.hc2AG,
                    StateSpace<ChildModel>().base.NS, no_output_years),
        hc1_art_pop(StateSpace<ChildModel>().base.hTS,
                    StateSpace<ChildModel>().children.hc1DS,
                    StateSpace<ChildModel>().children.hc1AG,
                    StateSpace<ChildModel>().base.NS, no_output_years),
        hc2_art_pop(StateSpace<ChildModel>().base.hTS,
                    StateSpace<ChildModel>().children.hc2DS,
                    StateSpace<ChildModel>().children.hc2AG,
                    StateSpace<ChildModel>().base.NS, no_output_years),
        hc1_noart_aids_deaths(StateSpace<ChildModel>().children.hc1DS,
                              StateSpace<ChildModel>().children.hcTT,
                              StateSpace<ChildModel>().children.hc1AG,
                              StateSpace<ChildModel>().base.NS, no_output_years),
        hc2_noart_aids_deaths(StateSpace<ChildModel>().children.hc2DS,
                              StateSpace<ChildModel>().children.hcTT,
                              StateSpace<ChildModel>().children.hc2AG,
                              StateSpace<ChildModel>().base.NS, no_output_years),
        hc1_art_aids_deaths(StateSpace<ChildModel>().base.hTS,
                            StateSpace<ChildModel>().children.hc1DS,
                            StateSpace<ChildModel>().children.hc1AG,
                            StateSpace<ChildModel>().base.NS, no_output_years),
        hc2_art_aids_deaths(StateSpace<ChildModel>().base.hTS,
                            StateSpace<ChildModel>().children.hc2DS,
                            StateSpace<ChildModel>().children.hc2AG,
                            StateSpace<ChildModel>().base.NS, no_output_years),
        hc_art_num(no_output_years) {
    hc1_hiv_pop.setZero();
    hc2_hiv_pop.setZero();
    hc1_art_pop.setZero();
    hc2_art_pop.setZero();
    hc1_noart_aids_deaths.setZero();
    hc2_noart_aids_deaths.setZero();
    hc1_art_aids_deaths.setZero();
    hc2_art_aids_deaths.setZero();
    hc_art_num.setZero();
  }
};

template<typename ModelVariant, typename real_type>
struct OutputState {
  BaseModelOutputState<ModelVariant, real_type> base;
  ChildModelOutputState<ModelVariant, real_type> children;

  OutputState(int no_output_years) :
      base(no_output_years),
      children(no_output_years) {}
};

template<typename ModelVariant, typename real_type>
struct BaseModelStateSaver {
public:
  void save_state(BaseModelOutputState<ModelVariant, real_type> &base_state,
                  const size_t i,
                  const State<ModelVariant, real_type> &state) {
    base_state.p_total_pop.chip(i, base_state.p_total_pop.NumDimensions - 1) = state.base.p_total_pop;
    base_state.p_total_pop_natural_deaths.chip(i, base_state.p_total_pop_natural_deaths.NumDimensions -
                                                  1) = state.base.p_total_pop_natural_deaths;
    base_state.p_hiv_pop.chip(i, base_state.p_hiv_pop.NumDimensions - 1) = state.base.p_hiv_pop;
    base_state.p_hiv_pop_natural_deaths.chip(i, base_state.p_hiv_pop_natural_deaths.NumDimensions -
                                                1) = state.base.p_hiv_pop_natural_deaths;
    base_state.h_hiv_adult.chip(i, base_state.h_hiv_adult.NumDimensions - 1) = state.base.h_hiv_adult;
    base_state.h_art_adult.chip(i, base_state.h_art_adult.NumDimensions - 1) = state.base.h_art_adult;
    base_state.births(i) = state.base.births;
    base_state.h_hiv_deaths_no_art.chip(i, base_state.h_hiv_deaths_no_art.NumDimensions -
                                           1) = state.base.h_hiv_deaths_no_art;
    base_state.p_infections.chip(i, base_state.p_infections.NumDimensions - 1) = state.base.p_infections;
    base_state.h_hiv_deaths_art.chip(i, base_state.h_hiv_deaths_art.NumDimensions -
                                        1) = state.base.h_hiv_deaths_art;
    base_state.h_art_initiation.chip(i, base_state.h_art_initiation.NumDimensions -
                                        1) = state.base.h_art_initiation;
    base_state.p_hiv_deaths.chip(i, base_state.p_hiv_deaths.NumDimensions - 1) = state.base.p_hiv_deaths;
    return;
  }
};

template<typename ModelVariant, typename real_type>
class ChildModelStateSaver {
public:
  void save_state(ChildModelOutputState<ModelVariant, real_type> &full_state,
                  const size_t i,
                  const State<ModelVariant, real_type> &state) {}
};

template<typename real_type>
class ChildModelStateSaver<ChildModel, real_type> {
public:
  void save_state(ChildModelOutputState<ChildModel, real_type> &children_state,
                  const size_t i,
                  const State<ChildModel, real_type> &state) {
    children_state.hc1_hiv_pop.chip(i, children_state.hc1_hiv_pop.NumDimensions - 1) =
        state.children.hc1_hiv_pop;
    children_state.hc2_hiv_pop.chip(i, children_state.hc2_hiv_pop.NumDimensions - 1) =
        state.children.hc2_hiv_pop;
    children_state.hc1_art_pop.chip(i, children_state.hc1_art_pop.NumDimensions - 1) =
        state.children.hc1_art_pop;
    children_state.hc2_art_pop.chip(i, children_state.hc2_art_pop.NumDimensions - 1) =
        state.children.hc2_art_pop;
    children_state.hc1_noart_aids_deaths.chip(i, children_state.hc1_noart_aids_deaths.NumDimensions - 1) =
      state.children.hc1_noart_aids_deaths;
    children_state.hc2_noart_aids_deaths.chip(i, children_state.hc2_noart_aids_deaths.NumDimensions - 1) =
      state.children.hc2_noart_aids_deaths;
    children_state.hc1_art_aids_deaths.chip(i, children_state.hc1_art_aids_deaths.NumDimensions - 1) =
      state.children.hc1_art_aids_deaths;
    children_state.hc2_art_aids_deaths.chip(i, children_state.hc2_art_aids_deaths.NumDimensions - 1) =
      state.children.hc2_art_aids_deaths;
  }
};


template<typename ModelVariant, typename real_type>
class StateSaver {
public:
  BaseModelStateSaver<ModelVariant, real_type> base;
  ChildModelStateSaver<ModelVariant, real_type> children;

  StateSaver(int time_steps,
             std::vector<int> save_steps) :
      save_steps(save_steps),
      full_state(save_steps.size()) {
    for (int step: save_steps) {
      if (step < 0) {
        std::stringstream ss;
        ss << "Output step must be at least 0, got '" << step << "'." << std::endl;
        throw std::runtime_error(ss.str());
      }
      if (step > time_steps) {
        std::stringstream ss;
        ss << "Output step can be at most number of time steps run which is '" << time_steps << "', got step '" << step
           << "'." << std::endl;
        throw std::runtime_error(ss.str());
      }
    }
  }


  void save_state(const State<ModelVariant, real_type> &state, int current_year) {
    for (size_t i = 0; i < save_steps.size(); ++i) {
      if (current_year == save_steps[i]) {
        base.save_state(full_state.base, i, state);
        children.save_state(full_state.children, i, state);
      }
    }
  }

  const OutputState<ModelVariant, real_type> &get_full_state() const {
    return full_state;
  }

private:
  std::vector<int> save_steps;
  OutputState<ModelVariant, real_type> full_state;
};

}

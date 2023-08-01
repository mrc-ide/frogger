#pragma once

#include "types.hpp"

namespace leapfrog {

template<HivAgeStratification S, typename real_type>
class StateSaver {
public:
  struct OutputState {
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
    Tensor5<real_type> hc1_hiv_pop;
    Tensor5<real_type> hc2_hiv_pop;

    OutputState(int pAG,
                int NS,
                int hDS,
                int hAG,
                int hTS,
                int no_output_years)
        : p_total_pop(StateSpace<S>().pAG, StateSpace<S>().NS, no_output_years),
          p_total_pop_natural_deaths(StateSpace<S>().pAG, StateSpace<S>().NS, no_output_years),
          p_hiv_pop(StateSpace<S>().pAG, StateSpace<S>().NS, no_output_years),
          p_hiv_pop_natural_deaths(StateSpace<S>().pAG, StateSpace<S>().NS, no_output_years),
          h_hiv_adult(StateSpace<S>().hDS, StateSpace<S>().hAG, StateSpace<S>().NS,
                          no_output_years),
          h_art_adult(StateSpace<S>().hTS,
                          StateSpace<S>().hDS,
                          StateSpace<S>().hAG,
                          StateSpace<S>().NS,
                          no_output_years),
          births(no_output_years),

          h_hiv_deaths_no_art(StateSpace<S>().hDS, StateSpace<S>().hAG,
                             StateSpace<S>().NS, no_output_years),
          p_infections(StateSpace<S>().pAG, StateSpace<S>().NS, no_output_years),
          h_hiv_deaths_art(StateSpace<S>().hTS, StateSpace<S>().hDS,
                          StateSpace<S>().hAG, StateSpace<S>().NS, no_output_years),
          h_art_initiation(StateSpace<S>().hDS, StateSpace<S>().hAG,
                         StateSpace<S>().NS, no_output_years),
          p_hiv_deaths(StateSpace<S>().pAG, StateSpace<S>().NS, no_output_years),
          hc1_hiv_pop(StateSpace<S>().hc1_disease_stages, StateSpace<S>().hTM, StateSpace<S>().hc1_age_groups,
                      StateSpace<S>().num_genders, no_output_years),
                      hc2_hiv_pop(StateSpace<S>().hc2_disease_stages, StateSpace<S>().hTM, StateSpace<S>().hc2_age_groups,
                                  StateSpace<S>().num_genders, no_output_years) {
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
      hc1_hiv_pop.setZero();
      hc2_hiv_pop.setZero();
    }
  };

  StateSaver(int time_steps,
             std::vector<int> save_steps) :
      save_steps(save_steps),
      full_state(StateSpace<S>().pAG, StateSpace<S>().NS, StateSpace<S>().hDS,
                 StateSpace<S>().hAG, StateSpace<S>().hTS, StateSpace<S>().hc1DS, StateSpace<S>().hc2DS, StateSpace<S>().hc1AG, StateSpace<S>().hc2AG, save_steps.size()) {
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


  void save_state(const State<S, real_type> &state, int current_year) {
    for (size_t i = 0; i < save_steps.size(); ++i) {
      if (current_year == save_steps[i]) {
        full_state.p_total_pop.chip(i, full_state.p_total_pop.NumDimensions - 1) = state.p_total_pop;
        full_state.p_total_pop_natural_deaths.chip(i, full_state.p_total_pop_natural_deaths.NumDimensions - 1) = state.p_total_pop_natural_deaths;
        full_state.p_hiv_pop.chip(i, full_state.p_hiv_pop.NumDimensions - 1) = state.p_hiv_pop;
        full_state.p_hiv_pop_natural_deaths.chip(i, full_state.p_hiv_pop_natural_deaths.NumDimensions - 1) =
            state.p_hiv_pop_natural_deaths;
        full_state.h_hiv_adult.chip(i, full_state.h_hiv_adult.NumDimensions - 1) = state.h_hiv_adult;
        full_state.h_art_adult.chip(i, full_state.h_art_adult.NumDimensions - 1) = state.h_art_adult;
        full_state.births(i) = state.births;
        full_state.h_hiv_deaths_no_art.chip(i, full_state.h_hiv_deaths_no_art.NumDimensions - 1) =
            state.h_hiv_deaths_no_art;
        full_state.p_infections.chip(i, full_state.p_infections.NumDimensions - 1) = state.p_infections;
        full_state.h_hiv_deaths_art.chip(i, full_state.h_hiv_deaths_art.NumDimensions - 1) = state.h_hiv_deaths_art;
        full_state.h_art_initiation.chip(i, full_state.h_art_initiation.NumDimensions - 1) = state.h_art_initiation;
        full_state.p_hiv_deaths.chip(i, full_state.p_hiv_deaths.NumDimensions - 1) = state.p_hiv_deaths;
        full_state.hc1_hiv_pop.chip(i, full_state.hc1_hiv_pop.NumDimensions - 1) = state.hc1_hiv_pop;
        full_state.hc2_hiv_pop.chip(i, full_state.hc2_hiv_pop.NumDimensions - 1) = state.hc2_hiv_pop;
        return;
      }
    }
  }

  const OutputState &get_full_state() const {
    return full_state;
  }

private:
  std::vector<int> save_steps;
  OutputState full_state;
};

}

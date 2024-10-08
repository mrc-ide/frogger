#pragma once

#include "general_demographic_projection.hpp"
#include "hiv_demographic_projection.hpp"
#include "model_simulation.hpp"
#include "child_model_simulation.hpp"
#include "state_saver.hpp"
#include "state_space.hpp"

namespace leapfrog {

// If we want to set any state for first iteration to something
// other than 0 do it here.
template<typename ModelVariant, typename real_type>
void set_initial_state(State<ModelVariant, real_type> &state,
                       const Parameters<ModelVariant, real_type> &pars) {
  constexpr auto ss_b = StateSpace<ModelVariant>().base;
  const auto& p_dm = pars.base.demography;

  for (int g = 0; g < ss_b.NS; ++g) {
    for (int a = 0; a < ss_b.pAG; ++a) {
      state.base.p_total_pop(a, g) = p_dm.base_pop(a, g);
    }
  }
}

/**
 * @brief Run a simulation model over a specified number of time steps.
 *
 * @tparam ModelVariant The variant of the model to be run, used for compile time switching.
 * @tparam real_type The data type used for real numbers in the simulation, usually a double.
 * @param time_steps The total number of time steps to run the simulation for.
 * @param save_steps A vector containing step intervals at which to save states.
 * @param pars The parameters required for running the simulation.
 * @return An OutputState object containing output from every save_step in the simulation.
 */
template<typename ModelVariant, typename real_type>
OutputState<ModelVariant, real_type> run_model(int time_steps,
                                               std::vector<int> save_steps,
                                               const Parameters<ModelVariant, real_type> &pars) {
  const auto& p_op = pars.base.options;

  auto state = State<ModelVariant, real_type>(pars);
  auto state_next = state;
  set_initial_state<ModelVariant, real_type>(state, pars);

  internal::IntermediateData<ModelVariant, real_type> intermediate(p_op.hAG_15plus);

  intermediate.reset();

  StateSaver<ModelVariant, real_type> state_output(time_steps, save_steps);
  // Save initial state
  state_output.save_state(state, 0);

  // Each time step is mid-point of the year
  for (int step = 1; step < time_steps; ++step) {
    run_general_pop_demographic_projection<ModelVariant>(step, pars, state, state_next,
                                                         intermediate);
    run_hiv_pop_demographic_projection<ModelVariant>(step, pars, state, state_next,
                                                     intermediate);
    run_hiv_model_simulation<ModelVariant>(step, pars, state, state_next, intermediate);
    if constexpr (ModelVariant::run_child_model) {
      run_child_model_simulation<ModelVariant>(step, pars, state, state_next, intermediate);
    }

    if (p_op.proj_period_int == internal::PROJPERIOD_CALENDAR) {
      run_end_year_migration<ModelVariant>(step, pars, state, state_next, intermediate);    
      run_hiv_pop_end_year_migration<ModelVariant>(step, pars, state, state_next, intermediate);
    }
    
    state_output.save_state(state_next, step);
    std::swap(state, state_next);
    state_next.reset();
    intermediate.reset();
  }
  return state_output.get_full_state();
}

}

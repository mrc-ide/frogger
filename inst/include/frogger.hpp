#pragma once

#include "general_demographic_projection.hpp"
#include "hiv_demographic_projection.hpp"
#include "model_simulation.hpp"

namespace leapfrog {

namespace internal {

template<typename real_type>
void initialise_model_state(const Parameters<real_type> &pars,
                            State<real_type> &state) {
  for (int g = 0; g < pars.num_genders; g++) {
    for (int a = 0; a < pars.age_groups_pop; a++) {
      state.total_population(a, g) = pars.base_pop(a, g);
    }
  }
  state.natural_deaths.setZero();
  state.hiv_population.setZero();
  state.hiv_natural_deaths.setZero();
  state.hiv_strat_adult.setZero();
  state.art_strat_adult.setZero();
  state.births = 0;
  state.aids_deaths_no_art.setZero();
  state.infections.setZero();
  state.aids_deaths_art.setZero();
  state.art_initiation.setZero();
  state.hiv_deaths.setZero();
}

}

template<typename real_type>
State<real_type> run_model(int time_steps, const Parameters<real_type> &pars) {
  State<real_type> state(pars.age_groups_pop, pars.num_genders,
                         pars.disease_stages, pars.age_groups_hiv,
                         pars.treatment_stages);

  internal::initialise_model_state(pars, state);
  auto state_next = state;
  internal::IntermediateData<real_type> intermediate(pars.age_groups_pop, pars.age_groups_hiv, pars.num_genders,
                                                     pars.disease_stages, pars.treatment_stages,
                                                     pars.age_groups_hiv_15plus);
  // Each time step is mid-point of the year
  for (int step = 1; step <= time_steps; ++step) {
    // AIDS deaths not on ART, infections and AIDS deaths are calculated fresh every year. They do not
    // depend on previous time step, so reset it to 0.
    state_next.aids_deaths_no_art.setZero();
    state_next.infections.setZero();
    state_next.hiv_deaths.setZero();
    internal::run_general_pop_demographic_projection(step, pars, state, state_next, intermediate);
    internal::run_hiv_pop_demographic_projection(step, pars, state, state_next, intermediate);
    internal::run_hiv_model_simulation(step, pars, state, state_next, intermediate);
    std::swap(state, state_next);
    intermediate.reset();
  }
  return state;
}

}

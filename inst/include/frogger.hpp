#pragma once

#include "generated/model_variants.hpp"
#include "generated/config_mixer.hpp"
#include "models/general_demographic_projection.hpp"
#include "models/hiv_demographic_projection.hpp"
#include "models/adult_hiv_model_simulation.hpp"
#include "models/child_model_simulation.hpp"
#include "options.hpp"

namespace leapfrog {

template<Language L, typename real_type, internal::MV ModelVariant>
struct Leapfrog {
  using Cfg = internal::Config<L, real_type, ModelVariant>;
  using SS = Cfg::SS;
  using Pars = Cfg::Pars;
  using State = Cfg::State;
  using Intermediate = Cfg::Intermediate;
  using OutputState = Cfg::OutputState;
  using Args = Cfg::Args;

  static OutputState run_model(
    const Pars& pars,
    const Options<real_type>& opts,
    const std::optional<State> initial_state,
    const std::optional<int> start_from_year,
    const std::vector<int> output_years
  ) {
    int start_from = start_from_year.value_or(opts.proj_start_year);

    State state;
    if (initial_state) {
      state = initial_state.value();
    } else {
      state = {};
      state.reset();
      if constexpr (ModelVariant::run_demographic_projection) {
        state.dp.p_total_pop = pars.dp.base_pop;
      }
    }

    auto state_next = state;
    state_next.reset();

    Intermediate intermediate;
    intermediate.reset();

    OutputState output_state(output_years.size());
    save_state(opts.proj_start_year, state, output_state, output_years);

    // Each time step is mid-point of the year
    for (int step = start_from - opts.proj_start_year + 1; step < opts.proj_time_steps; ++step) {
      Args args = { step, pars, state, state_next, intermediate, opts };
      project_year(args);
      save_state(opts.proj_start_year + step, state_next,
                 output_state, output_years);
      std::swap(state, state_next);
      state_next.reset();
      intermediate.reset();
    }
    return output_state;
  };

  private:
  static void save_state(
    const int step,
    State& state_next,
    OutputState& output_state,
    const std::vector<int>& output_years
  ) {
    for (size_t i = 0; i < output_years.size(); ++i) {
      if (step == output_years[i]) {
        output_state.save_state(i, state_next);
      }
    }
  };

  static void project_year(Args& args) {
    internal::GeneralDemographicProjection<Cfg> general_dp(args);
    internal::HivDemographicProjection<Cfg> hiv_dp(args);
    internal::AdultHivModelSimulation<Cfg> hiv_sim(args);
    internal::ChildModelSimulation<Cfg> child_sim(args);

    if constexpr (ModelVariant::run_demographic_projection) {
      general_dp.run_general_pop_demographic_projection();

      if constexpr (ModelVariant::run_hiv_simulation) {
        hiv_dp.run_hiv_pop_demographic_projection();
        hiv_sim.run_hiv_model_simulation();
      }

      if constexpr (ModelVariant::run_child_model) {
        child_sim.run_child_model_simulation();
      }

      if (args.opts.proj_period_int == SS::PROJPERIOD_CALENDAR) {
        general_dp.run_end_year_migration();

        if constexpr (ModelVariant::run_hiv_simulation) {
          hiv_dp.run_hiv_pop_end_year_migration();
        }
      }
    }
  };
};

}

#pragma once

#include "types.hpp"

namespace leapfrog {

namespace internal {

template<HivAgeStratification S, typename real_type>
void run_hiv_child_infections(int time_step,
                              const Parameters<real_type> &pars,
                              const State<S, real_type> &state_curr,
                              State<S, real_type> &state_next,
                              IntermediateData<S, real_type> &intermediate) {
  constexpr auto ss = StateSpace<S>();
  const auto children = pars.children;

  for (int g = 0; g < ss.num_genders; ++g) {
    // Run only first 5 age groups in total population 0, 1, 2, 3, 4
    for (int af = 0; af < 5; ++af) {
      if (children.hc_nosocomial(time_step) > 0) {
        // Divide by 10 because we want to evenly distribute over 2 genders and 5 age groups
        state_next.infections(af, g) = children.hc_nosocomial(time_step) / (5 * ss.num_genders);
        state_next.hiv_population(af, g) += state_next.infections(af, g);

        for (int hm = 0; hm < ss.disease_stages; ++hm) {
          // putting them all in perinatal hTM to match spec nosocomial
          if (children.hc1_cd4_dist(hm) > 0) {
            state_next.hc_hiv_pop(hm, 0, af, g) += state_next.infections(af, g) * children.hc1_cd4_dist(hm);
          }
        }
      }
    }
  }
}
}

template<HivAgeStratification S, typename real_type>
void run_child_model_simulation(int time_step,
                                const Parameters<real_type> &pars,
                                const State<S, real_type> &state_curr,
                                State<S, real_type> &state_next,
                                internal::IntermediateData<S, real_type> &intermediate) {
  internal::run_hiv_child_infections(time_step, pars, state_curr, state_next, intermediate);
}

}

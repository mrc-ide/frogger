#pragma once

#include "types.hpp"

namespace leapfrog {

namespace internal {

template<typename real_type>
void run_hiv_model_simulation(int time_step,
                              const Parameters<real_type> &pars,
                              const State<real_type> &state_curr,
                              State<real_type> &state_next,
                              IntermediateData<real_type> &intermediate) {
  run_add_new_hiv_infections(time_step, pars, state_curr, state_next, intermediate);

  intermediate.everARTelig_idx =
      pars.artcd4elig_idx(time_step) < pars.disease_stages ? pars.artcd4elig_idx(time_step) : pars.disease_stages;
  for (int hiv_step = 0; hiv_step < pars.hiv_steps_per_year; ++hiv_step) {
    run_disease_progression_and_mortality(hiv_step, time_step, pars, state_curr, state_next, intermediate);
    run_new_infections(hiv_step, time_step, pars, state_curr, state_next, intermediate);
  }
}

template<typename real_type>
void run_add_new_hiv_infections(int time_step,
                                const Parameters<real_type> &pars,
                                const State<real_type> &state_curr,
                                State<real_type> &state_next,
                                IntermediateData<real_type> &intermediate) {
  // TODO: Add EPP_DIRECTINCID switch (ask Jeff)

  // Calculating new infections once per year (like Spectrum)
  for (int g = 0; g < pars.num_genders; ++g) {
    for (int a = pars.adult_incidence_first_age_group; a < pars.age_groups_pop; a++) {
      intermediate.hiv_negative_pop(a, g) = state_curr.total_population(a, g) - state_curr.hiv_population(a, g);
    }
  }

  for (int g = 0; g < pars.num_genders; g++) {
    for (int a = pars.adult_incidence_first_age_group;
         a < pars.adult_incidence_first_age_group + pars.pAG_INCIDPOP; a++) {
      intermediate.hiv_neg_aggregate(g) += intermediate.hiv_negative_pop(a, g);
      intermediate.Xhivn_incagerr(g) +=
          pars.incidence_relative_risk_age(a - pars.adult_incidence_first_age_group, g, time_step) *
          intermediate.hiv_negative_pop(a, g);
    }
  }

  intermediate.incidence_rate_sex(MALE) =
      pars.incidence_rate(time_step) * (intermediate.hiv_neg_aggregate(MALE) + intermediate.hiv_neg_aggregate(FEMALE)) /
      (intermediate.hiv_neg_aggregate(MALE) +
       pars.incidence_relative_risk_sex(time_step) * intermediate.hiv_neg_aggregate(FEMALE));
  intermediate.incidence_rate_sex(FEMALE) =
      pars.incidence_rate(time_step) * pars.incidence_relative_risk_sex(time_step) *
      (intermediate.hiv_neg_aggregate(MALE) + intermediate.hiv_neg_aggregate(FEMALE)) /
      (intermediate.hiv_neg_aggregate(MALE) +
       pars.incidence_relative_risk_sex(time_step) * intermediate.hiv_neg_aggregate(FEMALE));

  for (int g = 0; g < pars.num_genders; g++) {
    for (int a = pars.hiv_adult_first_age_group; a < pars.age_groups_pop; a++) {
      intermediate.infections_ts(a, g) =
          intermediate.hiv_negative_pop(a, g) * intermediate.incidence_rate_sex(g) *
          pars.incidence_relative_risk_age(a - pars.adult_incidence_first_age_group, g, time_step) *
          intermediate.hiv_neg_aggregate(g) /
          intermediate.Xhivn_incagerr(g);
    }
  }
}

template<typename real_type>
void run_disease_progression_and_mortality(int hiv_step,
                                           int time_step,
                                           const Parameters<real_type> &pars,
                                           const State<real_type> &state_curr,
                                           State<real_type> &state_next,
                                           IntermediateData<real_type> &intermediate) {
  for (int g = 0; g < pars.num_genders; g++) {
    for (int ha = 0; ha < pars.age_groups_hiv; ++ha) {
      for (int hm = 0; hm < pars.disease_stages; ++hm) {
        // TODO: Mortality scaling not yet implemented
        if (pars.scale_cd4_mortality &&
            (time_step >= pars.time_art_start) &&
            (hm >= intermediate.everARTelig_idx) &&
            (state_next.hiv_strat_adult(hm, ha, g) > 0.0)) {
          for (int hu = 0; hu < pars.treatment_stages; ++hu) {
            intermediate.artpop_hahm += state_next.art_strat_adult(hu, hm, ha, g);
          }
          intermediate.cd4mx_scale = state_next.hiv_strat_adult(hm, ha, g) /
                                     (state_next.hiv_strat_adult(hm, ha, g) + intermediate.artpop_hahm);
        }

        intermediate.deaths =
            intermediate.cd4mx_scale * pars.cd4_mortality(hm, ha, g) * state_next.hiv_strat_adult(hm, ha, g);
        intermediate.hiv_deaths_age_sex(ha, g) += pars.dt * intermediate.deaths;
        state_next.aids_deaths_no_art(hm, ha, g) += pars.dt * intermediate.deaths;
        intermediate.grad(hm, ha, g) = -intermediate.deaths;
      }

      for (int hm = 1; hm < pars.disease_stages; ++hm) {
        intermediate.grad(hm - 1, ha, g) -=
            pars.cd4_progression(hm - 1, ha, g) * state_next.hiv_strat_adult(hm - 1, ha, g);
        intermediate.grad(hm, ha, g) += pars.cd4_progression(hm - 1, ha, g) * state_next.hiv_strat_adult(hm - 1, ha, g);
      }
    }
  }
}

template<typename real_type>
void run_new_infections(int hiv_step,
                        int time_step,
                        const Parameters<real_type> &pars,
                        const State<real_type> &state_curr,
                        State<real_type> &state_next,
                        IntermediateData<real_type> &intermediate) {
  for (int g = 0; g < pars.num_genders; g++) {
    int a = pars.hiv_adult_first_age_group;
    for (int ha = 0; ha < pars.age_groups_hiv; ++ha) {
      for (int i = 0; i < pars.hiv_age_groups_span(ha); i++, a++) {
        intermediate.infections_a = intermediate.infections_ts(a, g);
        intermediate.infections_ha += intermediate.infections_a;
        state_next.infections(a, g) += pars.dt * intermediate.infections_a;
        state_next.hiv_population(a, g) += pars.dt * intermediate.infections_a;
      }

      // add infections to grad hivpop
      for (int hm = 0; hm < pars.disease_stages; ++hm) {
        intermediate.grad(hm, ha, g) += intermediate.infections_ha * pars.cd4_initdist(hm, ha, g);
      }
    }
  }
}

}
}

#pragma once

#include "intermediate_data.hpp"

namespace leapfrog {

namespace internal {

template<typename ModelVariant, typename real_type>
void run_child_ageing(int t,
                      const Parameters<ModelVariant, real_type> &pars,
                      const State<ModelVariant, real_type> &state_curr,
                      State<ModelVariant, real_type> &state_next,
                      IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss_b = StateSpace<ModelVariant>().base;
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_dm = pars.base.demography;
  const auto& p_hc = pars.children.children;
  const auto& p_op = pars.base.options;
  const auto& c_hc = state_curr.children;
  auto& n_hc = state_next.children;

  for (int s = 0; s < ss_b.NS; ++s) {
    // less than 5 because there is a cd4 transition between ages 4 and 5
    for (int a = 1; a < ss_c.hc2_agestart; ++a) {
      for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
        for (int cat = 0; cat < ss_c.hcTT; ++cat) {
          n_hc.hc1_hiv_pop(hd, cat, a, s) += c_hc.hc1_hiv_pop(hd, cat, a - 1, s) * p_dm.survival_probability(a, s, t);
        }
        for (int dur = 0; dur < ss_b.hTS; ++dur) {
          n_hc.hc1_art_pop(dur, hd, a, s) += c_hc.hc1_art_pop(dur, hd, a - 1, s) * p_dm.survival_probability(a, s, t);
        }
      }
    }
  }

  for (int s = 0; s < ss_b.NS; ++s) {
    for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
      for (int hd_alt = 0; hd_alt < ss_c.hc2DS; ++hd_alt) {
        for (int cat = 0; cat < ss_c.hcTT; ++cat) {
          n_hc.hc2_hiv_pop(hd_alt, cat, 0, s) += c_hc.hc1_hiv_pop(hd, cat, ss_c.hc1_ageend, s) *
                                                 p_dm.survival_probability(ss_c.hc2_agestart, s, t) *
                                                 p_hc.hc_cd4_transition(hd_alt, hd);
        }
        for (int dur = 0; dur < ss_b.hTS; ++dur) {
          n_hc.hc2_art_pop(dur, hd_alt, 0, s) += c_hc.hc1_art_pop(dur, hd, ss_c.hc1_ageend, s) *
                                                 p_dm.survival_probability(ss_c.hc2_agestart, s, t) *
                                                 p_hc.hc_cd4_transition(hd_alt, hd);
        }
      }
    }
  }

  for (int s = 0; s < ss_b.NS; ++s) {
    for (int a = (ss_c.hc2_agestart + 1); a < p_op.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < ss_c.hc2DS; ++hd) {
        for (int cat = 0; cat < ss_c.hcTT; ++cat) {
          n_hc.hc2_hiv_pop(hd, cat, a - ss_c.hc2_agestart, s) += c_hc.hc2_hiv_pop(hd, cat, a - ss_c.hc2_agestart - 1, s) *
                                                                 p_dm.survival_probability(a, s, t);
        }
        for (int dur = 0; dur < ss_b.hTS; ++dur) {
          n_hc.hc2_art_pop(dur, hd, a - ss_c.hc2_agestart, s) += c_hc.hc2_art_pop(dur, hd, a - ss_c.hc2_agestart - 1, s) *
                                                                 p_dm.survival_probability(a, s, t);
        }
      }
    }
  }

}

template<typename ModelVariant, typename real_type>
void run_wlhiv_births(int t,
                      const Parameters<ModelVariant, real_type> &pars,
                      const State<ModelVariant, real_type> &state_curr,
                      State<ModelVariant, real_type> &state_next,
                      IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_wlhiv_births can only be called for model variants where run_child_model is true");
  constexpr auto ss_b = StateSpace<ModelVariant>().base;
  const auto& p_hc = pars.children.children;
  const auto& p_dm = pars.base.demography;
  const auto& p_op = pars.base.options;
  const auto& c_ba = state_curr.base;
  auto& n_ba = state_next.base;
  auto& n_hc = state_next.children;
  auto& i_hc = intermediate.children;

  i_hc.asfr_sum = 0.0;
  for (int a = 0; a < p_op.p_fertility_age_groups; ++a) {
    i_hc.asfr_sum += p_dm.age_specific_fertility_rate(a, t);
  } // end a

  for (int a = 0; a < p_op.p_fertility_age_groups; ++a) {
    i_hc.nHIVcurr = 0.0;
    i_hc.nHIVlast = 0.0;
    i_hc.df = 0.0;

    for (int hd = 0; hd < ss_b.hDS; ++hd) {
      i_hc.nHIVcurr += n_ba.h_hiv_adult(hd, a, 1);
      i_hc.nHIVlast += c_ba.h_hiv_adult(hd, a, 1);
      for (int ht = 0; ht < ss_b.hTS; ++ht) {
        i_hc.nHIVcurr += n_ba.h_art_adult(ht, hd, a, 1);
        i_hc.nHIVlast += c_ba.h_art_adult(ht, hd, a, 1);
      } // end hTS
    } // end hDS

    i_hc.prev = i_hc.nHIVcurr / n_ba.p_total_pop(a + 15, 1);

    for (int hd = 0; hd < ss_b.hDS; ++hd) {
      i_hc.df += p_hc.local_adj_factor * p_hc.fert_mult_by_age(a) * p_hc.fert_mult_off_art(hd) *
                 (n_ba.h_hiv_adult(hd, a, 1) + c_ba.h_hiv_adult(hd, a, 1)) / 2;
      // women on ART less than 6 months use the off art fertility multiplier
      i_hc.df += p_hc.local_adj_factor * p_hc.fert_mult_by_age(a) * p_hc.fert_mult_off_art(hd) *
                 (n_ba.h_art_adult(0, hd, a, 1) + c_ba.h_art_adult(0, hd, a, 1)) / 2;
      for (int ht = 1; ht < ss_b.hTS; ++ht) {
        i_hc.df += p_hc.local_adj_factor * p_hc.fert_mult_on_art(a) *
                   (n_ba.h_art_adult(ht, hd, a, 1) + c_ba.h_art_adult(ht, hd, a, 1)) / 2;
      } // end hTS
    } // end hDS


    if (i_hc.nHIVcurr > 0) {
      auto midyear_fertileHIV = (i_hc.nHIVcurr + i_hc.nHIVlast) / 2;
      i_hc.df = i_hc.df / midyear_fertileHIV;
    } else {
      i_hc.df = 1;
    }

    for (int hd = 0; hd < ss_b.hDS; ++hd) {
      i_hc.df += p_hc.local_adj_factor * p_hc.fert_mult_by_age(a) * p_hc.fert_mult_off_art(hd) *
                 (n_ba.h_hiv_adult(hd, a, 1) + c_ba.h_hiv_adult(hd, a, 1)) / 2;
      // women on ART less than 6 months use the off art fertility multiplier
      i_hc.df += p_hc.local_adj_factor * p_hc.fert_mult_by_age(a) * p_hc.fert_mult_off_art(hd) *
                 (n_ba.h_art_adult(0, hd, a, 1) + c_ba.h_art_adult(0, hd, a, 1)) / 2;
      for (int ht = 1; ht < ss_b.hTS; ++ht) {
        i_hc.df += p_hc.local_adj_factor * p_hc.fert_mult_on_art(a) *
                   (n_ba.h_art_adult(ht, hd, a, 1) + c_ba.h_art_adult(ht, hd, a, 1)) / 2;
      } // end hTS
    } // end hDS

    auto midyear_fertileHIV = (i_hc.nHIVcurr + i_hc.nHIVlast) / 2;
    i_hc.birthsCurrAge = midyear_fertileHIV * p_hc.total_fertility_rate(t) *
                         i_hc.df / (i_hc.df * i_hc.prev + 1 - i_hc.prev) *
                         p_dm.age_specific_fertility_rate(a, t) / i_hc.asfr_sum ;
    i_hc.birthsHE += i_hc.birthsCurrAge;
    if (a < 9) {
      i_hc.births_HE_15_24 += i_hc.birthsCurrAge;
    }
  } // end a
  n_hc.hiv_births = i_hc.birthsHE;
}

template<typename ModelVariant, typename real_type>
void run_wlhiv_births_input_mat_prev(int t,
                                     const Parameters<ModelVariant, real_type> &pars,
                                     const State<ModelVariant, real_type> &state_curr,
                                     State<ModelVariant, real_type> &state_next,
                                     IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_wlhiv_births_input_mat_prev can only be called for model variants where run_child_model is true");
  const auto& p_hc = pars.children.children;
  auto& n_hc = state_next.children;

  n_hc.hiv_births = p_hc.mat_hiv_births(t);
}

template<typename ModelVariant, typename real_type>
void calc_hiv_negative_pop(int t,
                           const Parameters<ModelVariant, real_type> &pars,
                           const State<ModelVariant, real_type> &state_curr,
                           State<ModelVariant, real_type> &state_next,
                           IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_hiv_negative_pop can only be called for model variants where run_child_model is true");
  constexpr auto ss_b = StateSpace<ModelVariant>().base;
  const auto& p_dm = pars.base.demography;
  auto& n_ba = state_next.base;
  auto& i_hc = intermediate.children;

  for (int s = 0; s < ss_b.NS; ++s) {
    for (int a = 0; a < ss_b.hAG; ++a) {
      i_hc.p_hiv_neg_pop(a, s) = p_dm.base_pop(a, s) - n_ba.p_hiv_pop(a, s);
    }// end a
  }// end s
}

template<typename ModelVariant, typename real_type>
void adjust_hiv_births(int t,
                       const Parameters<ModelVariant, real_type> &pars,
                       const State<ModelVariant, real_type> &state_curr,
                       State<ModelVariant, real_type> &state_next,
                       IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "adjust_hiv_births can only be called for model variants where run_child_model is true");
  const auto& p_hc = pars.children.children;
  auto& n_hc = state_next.children;

  if (p_hc.abortion(t, 1) == 1) {
    n_hc.hiv_births -= n_hc.hiv_births * p_hc.abortion(t, 0);
  } else {
    n_hc.hiv_births -=  p_hc.abortion(t, 0);
  }
}

template<typename ModelVariant, typename real_type>
void convert_PMTCT_num_to_perc(int t,
                               const Parameters<ModelVariant, real_type> &pars,
                               const State<ModelVariant, real_type> &state_curr,
                               State<ModelVariant, real_type> &state_next,
                               IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "convert_PMTCT_num_to_perc can only be called for model variants where run_child_model is true");
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_hc = pars.children.children;
  auto& n_hc = state_next.children;
  auto& i_hc = intermediate.children;

  i_hc.sumARV = 0.0;
  for (int hp = 0; hp < ss_c.hPS; ++hp) {
    i_hc.sumARV += p_hc.PMTCT(hp, t);
  }

  i_hc.need_PMTCT = std::max(i_hc.sumARV, n_hc.hiv_births);

  i_hc.OnPMTCT = i_hc.sumARV + p_hc.patients_reallocated(t);
  i_hc.OnPMTCT = std::min(i_hc.OnPMTCT, i_hc.need_PMTCT);

  // replace all instances of coverage input as numbers with percentage covered
  if (p_hc.PMTCT_input_is_percent(t)) {
    for (int hp = 0; hp < ss_c.hPS; ++hp) {
      i_hc.PMTCT_coverage(hp) = p_hc.PMTCT(hp, t) / 100;
    } // end hPS
    i_hc.sumARV = i_hc.sumARV * i_hc.need_PMTCT;
  } else {
    for (int hp = 0; hp < ss_c.hPS; ++hp) {
      if (i_hc.sumARV == 0) {
        i_hc.PMTCT_coverage(hp) = 0.0;
      } else {
        i_hc.PMTCT_coverage(hp) = p_hc.PMTCT(hp, t) / i_hc.sumARV *
                                  i_hc.OnPMTCT / i_hc.need_PMTCT;
      }
    } // end hPS
  }

  i_hc.PMTCT_coverage(4) = i_hc.PMTCT_coverage(4) * p_hc.PMTCT_dropout(0, t);
  i_hc.PMTCT_coverage(5) = i_hc.PMTCT_coverage(5) * p_hc.PMTCT_dropout(1, t);
}

template<typename ModelVariant, typename real_type>
void convert_PMTCT_pre_bf(int t,
                          const Parameters<ModelVariant, real_type> &pars,
                          const State<ModelVariant, real_type> &state_curr,
                          State<ModelVariant, real_type> &state_next,
                          IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "convert_PMTCT_num_to_perc can only be called for model variants where run_child_model is true");
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_hc = pars.children.children;
  auto& i_hc = intermediate.children;

  // TODO: Maggie to confirm why Option A/B alt tr aren't used
  for (int hp = 0; hp < ss_c.hPS; ++hp) {
    i_hc.PMTCT_coverage(hp) *= 1.0 - p_hc.PMTCT_transmission_rate(4, hp, 0);
  } // end hPS
}

template<typename ModelVariant, typename real_type>
void calc_wlhiv_cd4_proportion(int t,
                               const Parameters<ModelVariant, real_type> &pars,
                               const State<ModelVariant, real_type> &state_curr,
                               State<ModelVariant, real_type> &state_next,
                               IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_wlhiv_cd4_proportion can only be called for model variants where run_child_model is true");
  const auto& p_hc = pars.children.children;
  auto& n_ba = state_next.base;
  auto& i_hc = intermediate.children;

  // Option A and B were only authorized for women with greater than 350 CD4, so if the percentage of women
  // on option A/B > the proportion of women in this cd4 category, we assume that some must have a cd4 less than 350
  // option AB will be less effective for these women so we adjust for that

  if (p_hc.mat_prev_input(t)) {
    i_hc.prop_wlhiv_lt200 = p_hc.prop_lt200(t);
    i_hc.prop_wlhiv_200to350 = 1.0 - p_hc.prop_gte350(t) - p_hc.prop_lt200(t);
    i_hc.prop_wlhiv_gte350 = p_hc.prop_gte350(t);
    i_hc.prop_wlhiv_lt350 = 1 - p_hc.prop_gte350(t);
    i_hc.num_wlhiv = p_hc.mat_hiv_births(t);
  } else {
    i_hc.num_wlhiv_lt200 = 0.0;
    i_hc.num_wlhiv_200to350 = 0.0;
    i_hc.num_wlhiv_gte350 = 0.0;
    i_hc.num_wlhiv = 0.0;
    i_hc.prop_wlhiv_lt200 = 0.0;
    i_hc.prop_wlhiv_200to350 = 0.0;
    i_hc.prop_wlhiv_gte350 = 0.0;
    i_hc.prop_wlhiv_lt350 = 0.0;

    for (int a = 0; a < 35; ++a) {
      i_hc.num_wlhiv_lt200 += n_ba.h_hiv_adult(4, a, 1) + n_ba.h_hiv_adult(5, a, 1) + n_ba.h_hiv_adult(6, a, 1);
      i_hc.num_wlhiv_200to350 += n_ba.h_hiv_adult(3, a, 1) + n_ba.h_hiv_adult(2, a, 1);
      i_hc.num_wlhiv_gte350 += n_ba.h_hiv_adult(0, a, 1) + n_ba.h_hiv_adult(1, a, 1);
    }
    i_hc.num_wlhiv = i_hc.num_wlhiv_200to350 + i_hc.num_wlhiv_gte350 + i_hc.num_wlhiv_lt200;

    if (i_hc.num_wlhiv > 0) {
      i_hc.prop_wlhiv_lt200 = i_hc.num_wlhiv_lt200 / i_hc.num_wlhiv;
      i_hc.prop_wlhiv_200to350 = i_hc.num_wlhiv_200to350 / i_hc.num_wlhiv;
      i_hc.prop_wlhiv_gte350 = i_hc.num_wlhiv_gte350 / i_hc.num_wlhiv;
    } else {
      i_hc.prop_wlhiv_lt200 = 0;
      i_hc.prop_wlhiv_200to350 = 1;
      i_hc.prop_wlhiv_gte350 = 0;
    }
    i_hc.prop_wlhiv_lt350 = i_hc.prop_wlhiv_lt200 + i_hc.prop_wlhiv_200to350;
  }
}

template<typename ModelVariant, typename real_type>
void adjust_option_A_B_tr(int t,
                          const Parameters<ModelVariant, real_type> &pars,
                          const State<ModelVariant, real_type> &state_curr,
                          State<ModelVariant, real_type> &state_next,
                          IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "adjust_option_A_B_tr can only be called for model variants where run_child_model is true");
  const auto& p_hc = pars.children.children;
  auto& i_hc = intermediate.children;

  // Option A and B were only authorized for women with greater than 350 CD4, so if the percentage of women
  // on option A/B > the proportion of women in this cd4 category, we assume that some must have a cd4 less than 350
  // option AB will be less effective for these women so we adjust for that
  internal::calc_wlhiv_cd4_proportion(t, pars, state_curr, state_next, intermediate);

  auto option_A_B_coverage = i_hc.PMTCT_coverage(0) + i_hc.PMTCT_coverage(1);
  if (option_A_B_coverage > i_hc.prop_wlhiv_gte350) {
    if (i_hc.prop_wlhiv_gte350 > 0) {
      i_hc.excessratio = option_A_B_coverage / i_hc.prop_wlhiv_gte350 - 1;
    } else {
      i_hc.excessratio = 0;
    }
    i_hc.optA_transmission_rate = p_hc.PMTCT_transmission_rate(0, 0, 0) * (1 + i_hc.excessratio);
    i_hc.optB_transmission_rate = p_hc.PMTCT_transmission_rate(0, 1, 0) * (1 + i_hc.excessratio);
  } else {
    i_hc.excessratio = 0.0;
    i_hc.optA_transmission_rate = p_hc.PMTCT_transmission_rate(0, 0, 0) * (1 + i_hc.excessratio);
    i_hc.optB_transmission_rate = p_hc.PMTCT_transmission_rate(0, 1, 0) * (1 + i_hc.excessratio);
  }
}

template<typename ModelVariant, typename real_type>
void adjust_option_A_B_bf_tr(int t,
                             const Parameters<ModelVariant, real_type> &pars,
                             const State<ModelVariant, real_type> &state_curr,
                             State<ModelVariant, real_type> &state_next,
                             IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "adjust_option_A_B_bf_tr can only be called for model variants where run_child_model is true");
  const auto& p_hc = pars.children.children;
  auto& i_hc = intermediate.children;

  internal::calc_wlhiv_cd4_proportion(t, pars, state_curr, state_next, intermediate);

  // Option A and B were only authorized for women with greater than 350 CD4, so if the percentage of women
  // on option A/B > the proportion of women in this cd4 category, we assume that some must have a cd4 less than 350
  // option AB will be less effective for these women so we adjust for that

  if (i_hc.prop_wlhiv_gte350 > 0) {
    auto option_A_B_coverage = i_hc.PMTCT_coverage(0) + i_hc.PMTCT_coverage(1);
    if (option_A_B_coverage > i_hc.prop_wlhiv_gte350) {
      i_hc.excessratio_bf = option_A_B_coverage - i_hc.prop_wlhiv_gte350;
      auto excess_factor_bf = i_hc.excessratio_bf / option_A_B_coverage * (1.45 / 0.46) +
                              i_hc.prop_wlhiv_gte350;
      i_hc.optA_bf_transmission_rate = excess_factor_bf * p_hc.PMTCT_transmission_rate(4, 0, 1);
      i_hc.optB_bf_transmission_rate = excess_factor_bf * p_hc.PMTCT_transmission_rate(4, 1, 1);
    } else {
      i_hc.optA_bf_transmission_rate = p_hc.PMTCT_transmission_rate(4, 0, 1);
      i_hc.optB_bf_transmission_rate = p_hc.PMTCT_transmission_rate(4, 1, 1);
    }
  } else {
    i_hc.optA_bf_transmission_rate = p_hc.PMTCT_transmission_rate(4, 0, 1);
    i_hc.optB_bf_transmission_rate = p_hc.PMTCT_transmission_rate(4, 1, 1);
  }
}

template<typename ModelVariant, typename real_type>
void maternal_incidence_in_pregnancy_tr(int t,
                                        const Parameters<ModelVariant, real_type> &pars,
                                        const State<ModelVariant, real_type> &state_curr,
                                        State<ModelVariant, real_type> &state_next,
                                        IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "maternal_incidence_in_pregnancy_tr can only be called for model variants where run_child_model is true");
  const auto& p_dm = pars.base.demography;
  const auto& p_op = pars.base.options;
  const auto& p_hc = pars.children.children;
  auto& n_ba = state_next.base;
  auto& i_hc = intermediate.children;

  // Transmission due to incident infections
  i_hc.asfr_sum = 0.0;
  for (int a = 0; a < p_op.p_fertility_age_groups; ++a) {
    i_hc.asfr_sum += p_dm.age_specific_fertility_rate(a, t);
  } // end a

  if (p_hc.mat_prev_input(t)) {
    for (int a = 0; a < p_op.p_fertility_age_groups; ++a) {
      auto asfr_weight = p_dm.age_specific_fertility_rate(a, t) / i_hc.asfr_sum;
      i_hc.age_weighted_hivneg += asfr_weight * p_hc.adult_female_hivnpop(a, t); // HIV negative 15-49 women weighted for ASFR
      i_hc.age_weighted_infections += asfr_weight * p_hc.adult_female_infections(a, t); // newly infected 15-49 women, weighted for ASFR
    } // end a

    if (i_hc.age_weighted_hivneg > 0.0) {
      i_hc.incidence_rate_wlhiv = i_hc.age_weighted_infections / i_hc.age_weighted_hivneg;
      // 0.75 is 9/12, gestational period, index 7 in the vertical trasnmission object is the index for maternal seroconversion
      i_hc.perinatal_transmission_from_incidence = i_hc.incidence_rate_wlhiv * 0.75 *
                                                   (p_hc.total_births(t) - i_hc.need_PMTCT) *
                                                   p_hc.vertical_transmission_rate(7, 0);
    } else {
      i_hc.incidence_rate_wlhiv = 0.0;
      i_hc.perinatal_transmission_from_incidence = 0.0;
    }
  } else {
    for (int a = 0; a < p_op.p_fertility_age_groups; ++a) {
      auto asfr_weight = p_dm.age_specific_fertility_rate(a, t) / i_hc.asfr_sum;
      i_hc.age_weighted_hivneg += asfr_weight * i_hc.p_hiv_neg_pop(a + 15, 1); // HIV negative 15-49 women weighted for ASFR
      i_hc.age_weighted_infections += asfr_weight * n_ba.p_infections(a + 15, 1); // newly infected 15-49 women, weighted for ASFR
    } // end a

    if (i_hc.age_weighted_hivneg > 0.0) {
      i_hc.incidence_rate_wlhiv = i_hc.age_weighted_infections / i_hc.age_weighted_hivneg;
      //0.75 is 9/12, gestational period, index 7 in the vertical trasnmission object is the index for maternal seroconversion
      i_hc.perinatal_transmission_from_incidence = i_hc.incidence_rate_wlhiv * 0.75 *
                                                   (n_ba.births - i_hc.need_PMTCT) *
                                                   p_hc.vertical_transmission_rate(7, 0);
    } else {
      i_hc.incidence_rate_wlhiv = 0.0;
      i_hc.perinatal_transmission_from_incidence = 0.0;
    }
  }
}

template<typename ModelVariant, typename real_type>
void perinatal_tr(int t,
                  const Parameters<ModelVariant, real_type> &pars,
                  const State<ModelVariant, real_type> &state_curr,
                  State<ModelVariant, real_type> &state_next,
                  IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "perinatal_tr can only be called for model variants where run_child_model is true");
  const auto& p_hc = pars.children.children;
  auto& n_ba = state_next.base;
  auto& i_hc = intermediate.children;

  i_hc.births_sum = n_ba.births;

  // TODO: add in patients reallocated
  internal::convert_PMTCT_num_to_perc(t, pars, state_curr, state_next, intermediate);
  internal::adjust_option_A_B_tr(t, pars, state_curr, state_next, intermediate);
  internal::calc_hiv_negative_pop(t, pars, state_curr, state_next, intermediate);

  // Calculate transmission rate
  i_hc.retained_on_ART = i_hc.PMTCT_coverage(4);
  i_hc.retained_started_ART = i_hc.PMTCT_coverage(5);

  // Transmission among women on treatment
  i_hc.perinatal_transmission_rate = i_hc.PMTCT_coverage(0) * i_hc.optA_transmission_rate +
                                     i_hc.PMTCT_coverage(1) * i_hc.optB_transmission_rate +
                                     i_hc.PMTCT_coverage(2) * p_hc.PMTCT_transmission_rate(0, 2, 0) + // SDNVP
                                     i_hc.PMTCT_coverage(3) * p_hc.PMTCT_transmission_rate(0, 3, 0) + //dual ARV
                                     i_hc.retained_on_ART * p_hc.PMTCT_transmission_rate(0, 4, 0) +
                                     i_hc.retained_started_ART * p_hc.PMTCT_transmission_rate(0, 5, 0) +
                                     i_hc.PMTCT_coverage(6) * p_hc.PMTCT_transmission_rate(0, 6, 0);

  i_hc.receiving_PMTCT = i_hc.PMTCT_coverage(0) + i_hc.PMTCT_coverage(1) +
                         i_hc.PMTCT_coverage(2) + i_hc.PMTCT_coverage(3) +
                         i_hc.retained_on_ART + i_hc.retained_started_ART +
                         i_hc.PMTCT_coverage(6);

  i_hc.no_PMTCT = 1 - i_hc.receiving_PMTCT;
  i_hc.no_PMTCT = std::max(i_hc.no_PMTCT, 0.0);

  // Transmission among women not on treatment
  if (i_hc.num_wlhiv > 0) {
    auto untreated_vertical_tr = i_hc.prop_wlhiv_lt200 * p_hc.vertical_transmission_rate(4, 0) +
                                 i_hc.prop_wlhiv_200to350 * p_hc.vertical_transmission_rate(2, 0) +
                                 i_hc.prop_wlhiv_gte350 * p_hc.vertical_transmission_rate(0, 0);
    i_hc.perinatal_transmission_rate += i_hc.no_PMTCT * untreated_vertical_tr;
  }
  i_hc.perinatal_transmission_rate_bf_calc = i_hc.perinatal_transmission_rate;

  internal::maternal_incidence_in_pregnancy_tr(t, pars, state_curr, state_next, intermediate);

  if (i_hc.need_PMTCT > 0.0) {
    i_hc.perinatal_transmission_rate = i_hc.perinatal_transmission_rate +
                                       i_hc.perinatal_transmission_from_incidence / i_hc.need_PMTCT;
  }
}

template<typename ModelVariant, typename real_type>
void maternal_incidence_in_bf_tr(int t,
                                 const Parameters<ModelVariant, real_type> &pars,
                                 const State<ModelVariant, real_type> &state_curr,
                                 State<ModelVariant, real_type> &state_next,
                                 IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "maternal_incidence_in_bf_tr can only be called for model variants where run_child_model is true");
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_hc = pars.children.children;
  auto& i_hc = intermediate.children;

  for (int bf = 0; bf < ss_c.hBF; ++bf) {
    i_hc.bf_at_risk += i_hc.incidence_rate_wlhiv / 12 * 2 *
                       (1 - p_hc.breastfeeding_duration_no_art(bf, t));
  }
   i_hc.bf_incident_hiv_transmission_rate = i_hc.bf_at_risk * p_hc.vertical_transmission_rate(7, 1);
}

template<typename ModelVariant, typename real_type>
void run_bf_transmission_rate(int t,
                              const Parameters<ModelVariant, real_type> &pars,
                              const State<ModelVariant, real_type> &state_curr,
                              State<ModelVariant, real_type> &state_next,
                              IntermediateData<ModelVariant, real_type> &intermediate,
                              int bf_start, int bf_end, int index) {
  static_assert(ModelVariant::run_child_model,
                "run_bf_transmission_rate can only be called for model variants where run_child_model is true");
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_hc = pars.children.children;
  auto& i_hc = intermediate.children;

  // set previous MTCT to zero at the start of each breastfeeding transmission
  for (int hp = 0; hp < ss_c.hPS; hp++) {
    i_hc.previous_mtct(hp) = 0.0;
  }

  for (int bf = bf_start; bf < bf_end; bf++) {
    // i_hc.perinatal_transmission_rate_bf_calc is the transmission that has already occurred due to perinatal transmission
    // i_hc.percent_no_treatment is the percentage of women who are still vulnerable to HIV transmission to their babies
    i_hc.percent_no_treatment = 1 - i_hc.perinatal_transmission_rate_bf_calc - i_hc.bf_transmission_rate(index) ;
    for (int bf = 0; bf < index; ++bf) {
      i_hc.percent_no_treatment -= i_hc.bf_transmission_rate(bf);
    }

    for (int hp = 0; hp < ss_c.hPS; hp++) {
      i_hc.percent_on_treatment = 0;
      // hp = 0 is option A
      if (hp == 0 && i_hc.PMTCT_coverage(hp) > 0) {
        if (bf < 6) {
          i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp) *
                                      pow(1 - p_hc.PMTCT_dropout(4, t) * 2, bf);
        } else {
          i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp) *
                                      pow(1 - p_hc.PMTCT_dropout(4, t) * 2, 5) *
                                      pow(1 - p_hc.PMTCT_dropout(5, t) * 2, bf - 5);
        }
        i_hc.percent_no_treatment -= i_hc.percent_on_treatment;
      }

      // hp = 1 is option B
      if (hp == 1 && i_hc.PMTCT_coverage(hp) > 0) {
        if (bf < 6) {
          i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp) *
                                      pow(1 - p_hc.PMTCT_dropout(4, t) * 2, bf);
        } else {
          i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp) *
                                      pow(1 - p_hc.PMTCT_dropout(4, t) * 2, 5) *
                                      pow(1 - p_hc.PMTCT_dropout(5, t) * 2, bf - 5);
        }
        i_hc.percent_no_treatment -=  i_hc.percent_on_treatment;
      }

      // sdnvp
      if (hp == 2 && i_hc.PMTCT_coverage(hp) > 0) {
        i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp);
        i_hc.percent_on_treatment -= i_hc.previous_mtct(hp);
        auto sdnvp_bf_tr = i_hc.percent_on_treatment *
                           p_hc.PMTCT_transmission_rate(0, hp, 1) *
                           2 * (1 - p_hc.breastfeeding_duration_art(bf, t));
        i_hc.previous_mtct(hp) += sdnvp_bf_tr;
        i_hc.bf_transmission_rate(index) += sdnvp_bf_tr;
        i_hc.percent_no_treatment -=  i_hc.percent_on_treatment;
      }

      // dual arv
      if (hp == 3 && i_hc.PMTCT_coverage(hp) > 0) {
        i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp);
        i_hc.percent_on_treatment -= i_hc.previous_mtct(hp);
        auto dual_arv_bf_tr = i_hc.percent_on_treatment *
                              p_hc.PMTCT_transmission_rate(4, hp, 1) *
                              2 * (1 - p_hc.breastfeeding_duration_art(bf, t));
        i_hc.previous_mtct(hp) += dual_arv_bf_tr;
        i_hc.bf_transmission_rate(index) += dual_arv_bf_tr;
        i_hc.percent_no_treatment -= i_hc.percent_on_treatment;
      }

      // on art pre preg
      if (hp > 3 && i_hc.PMTCT_coverage(hp) > 0) {
        if (bf < 6) {
          i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp) *
                                      pow(1 - p_hc.PMTCT_dropout(4, t) * 2, bf);
        } else {
          i_hc.percent_on_treatment = i_hc.PMTCT_coverage(hp) *
                                      pow(1 - p_hc.PMTCT_dropout(4, t) * 2, 5) *
                                      pow(1 - p_hc.PMTCT_dropout(5, t) * 2, bf - 5);
        }
        i_hc.percent_on_treatment -= i_hc.previous_mtct(hp);
        auto art_bf_tr = i_hc.percent_on_treatment *
                         p_hc.PMTCT_transmission_rate(4,hp,1) *
                         2 * (1 - p_hc.breastfeeding_duration_art(bf, t));
        i_hc.previous_mtct(hp) += art_bf_tr;
        i_hc.bf_transmission_rate(index) += art_bf_tr;
        i_hc.percent_no_treatment -= i_hc.percent_on_treatment;
      }
    }
  
    i_hc.percent_no_treatment = std::max(i_hc.percent_no_treatment, 0.0);

    // No treatment
    if (p_hc.breastfeeding_duration_no_art(bf, t) < 1) {
      auto untreated_vertical_bf_tr = i_hc.prop_wlhiv_lt200 * p_hc.vertical_transmission_rate(4, 1) +
                                      i_hc.prop_wlhiv_200to350 * p_hc.vertical_transmission_rate(2, 1) +
                                      i_hc.prop_wlhiv_gte350 * p_hc.vertical_transmission_rate(0, 1);
      i_hc.bf_transmission_rate(index) += i_hc.percent_no_treatment *
                                          untreated_vertical_bf_tr *
                                          2 * (1 - p_hc.breastfeeding_duration_no_art(bf, t));
    }

    if (bf < 1) {
      i_hc.bf_transmission_rate(index) /= 4.0;
    }
  }
}

template<typename ModelVariant, typename real_type>
void nosocomial_infections(int t,
                           const Parameters<ModelVariant, real_type> &pars,
                           const State<ModelVariant, real_type> &state_curr,
                           State<ModelVariant, real_type> &state_next,
                           IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "nosocomial_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss_b = StateSpace<ModelVariant>().base;
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_hc = pars.children.children;
  auto& n_ba = state_next.base;
  auto& n_hc = state_next.children;

  for (int s = 0; s < ss_b.NS; ++s) {
    // Run only first 5 age groups in total population 0, 1, 2, 3, 4
    for (int a = 0; a < ss_c.hc2_agestart; ++a) {
      if (p_hc.hc_nosocomial(t) > 0) {
        // 5.0 is used because we want to evenly distribute across the 5 age groups in 0-4
        n_ba.p_infections(a, s) = p_hc.hc_nosocomial(t) / (5.0 * ss_b.NS);
        n_ba.p_hiv_pop(a, s) += n_ba.p_infections(a, s);
        // Putting all nosocomial acquired HIV infections in perinatally acquired infection timing and highest CD4 category to match Spectrum implementation
        n_hc.hc1_hiv_pop(0, 0, a, s) += n_ba.p_infections(a, s);
      }
    } // end a
  } // end NS
}

template<typename ModelVariant, typename real_type>
void add_infections(int t,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "add_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss_b = StateSpace<ModelVariant>().base;
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_dm = pars.base.demography;
  const auto& p_hc = pars.children.children;
  auto& n_ba = state_next.base;
  auto& n_hc = state_next.children;
  auto& i_hc = intermediate.children;

  if (n_hc.hiv_births > 0) {
    internal::perinatal_tr(t, pars, state_curr, state_next, intermediate);

    // Perinatal transmission
    auto perinatal_transmission_births = n_hc.hiv_births * i_hc.perinatal_transmission_rate;
    for (int s = 0; s < ss_b.NS; ++s) {
      for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
        if (s == 0) {
          n_hc.hc1_hiv_pop(hd, 0, 0, 0) += perinatal_transmission_births *
                                           p_dm.births_sex_prop(0, t) * p_hc.hc1_cd4_dist(hd);
        } else {
          n_hc.hc1_hiv_pop(hd, 0, 0, s) += perinatal_transmission_births *
                                           (1 - p_dm.births_sex_prop(0, t)) * p_hc.hc1_cd4_dist(hd);
        }
      } // end hc1DS
      auto perinatal_births_by_sex = perinatal_transmission_births * p_dm.births_sex_prop(s, t);
      n_ba.p_hiv_pop(0, s) += perinatal_births_by_sex;
      n_ba.p_infections(0, s) += perinatal_births_by_sex;
    } // end NS

    // Breastfeeding transmission
    // 0-6
    internal::maternal_incidence_in_bf_tr(t, pars, state_curr, state_next, intermediate);
    internal::adjust_option_A_B_bf_tr(t, pars, state_curr, state_next, intermediate);
    internal::convert_PMTCT_pre_bf(t, pars, state_curr, state_next, intermediate);
    internal::run_bf_transmission_rate(t, pars, state_curr, state_next, intermediate, 0, 3, 0);

    real_type total_births = 0.0;
    if (p_hc.mat_prev_input(t)) {
      total_births = p_hc.total_births(t);
    } else {
      total_births = n_ba.births;
    }

    // 0-6
    for (int s = 0; s < ss_b.NS; ++s) {
      auto bf_hiv_by_sex = n_hc.hiv_births * p_dm.births_sex_prop(s, t) *
                           i_hc.bf_transmission_rate(0);
      // vertical infection from maternal infection during breastfeeding
      bf_hiv_by_sex += (total_births - n_hc.hiv_births) * p_dm.births_sex_prop(s, t) *
                       i_hc.bf_incident_hiv_transmission_rate;
      for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
        n_hc.hc1_hiv_pop(hd, 1, 0, s) += p_hc.hc1_cd4_dist(hd) * bf_hiv_by_sex;
      } // end hc1DS
      n_ba.p_hiv_pop(0, s) += bf_hiv_by_sex;
      n_ba.p_infections(0, s) += bf_hiv_by_sex;
    } // end NS

    // 6-12
    internal::run_bf_transmission_rate(t, pars, state_curr, state_next, intermediate, 3, 6, 1);
    for (int s = 0; s < ss_b.NS; ++s) {
      auto bf_hiv_by_sex = n_hc.hiv_births * p_dm.births_sex_prop(s, t) * i_hc.bf_transmission_rate(1);
      for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
        n_hc.hc1_hiv_pop(hd, 2, 0, s) += p_hc.hc1_cd4_dist(hd) * bf_hiv_by_sex;
      } // end hc1DS
      n_ba.p_hiv_pop(0, s) += bf_hiv_by_sex;
      n_ba.p_infections(0, s) += bf_hiv_by_sex;
    } // end NS

    // 12 plus
    internal::run_bf_transmission_rate(t, pars, state_curr, state_next, intermediate, 6, 12, 2);
    internal::run_bf_transmission_rate(t, pars, state_curr, state_next, intermediate, 12, ss_c.hBF, 3);
    for (int s = 0; s < ss_b.NS; ++s) {
      auto uninfected_prop_12_24 = (n_ba.p_total_pop(1, s) - n_ba.p_hiv_pop(1, s)) /
                                   (n_ba.p_total_pop(1, 0) - n_ba.p_hiv_pop(1, 0) + n_ba.p_total_pop(1, 1) - n_ba.p_hiv_pop(1, 1));
      auto uninfected_prop_24_plus = (n_ba.p_total_pop(2, s) - n_ba.p_hiv_pop(2, s)) /
                                     (n_ba.p_total_pop(2, 0) - n_ba.p_hiv_pop(2, 0) + n_ba.p_total_pop(2, 1) - n_ba.p_hiv_pop(2, 1));
      auto bf_hiv_transmission_12_24 = n_hc.hiv_births * i_hc.bf_transmission_rate(2) * uninfected_prop_12_24;
      auto bf_hiv_transmission_24_plus = n_hc.hiv_births * i_hc.bf_transmission_rate(3) * uninfected_prop_24_plus;
      for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
        // 12-24
        n_hc.hc1_hiv_pop(hd, 3, 1, s) += p_hc.hc1_cd4_dist(hd) * bf_hiv_transmission_12_24;
        n_ba.p_infections(1, s) += n_hc.hc1_hiv_pop(hd, 3, 1, s);

        // 24 plus
        auto new_hc1_infections_24_plus = p_hc.hc1_cd4_dist(hd) * bf_hiv_transmission_24_plus;
        n_hc.hc1_hiv_pop(hd, 3, 2, s) += new_hc1_infections_24_plus;
        n_ba.p_infections(2, s) += new_hc1_infections_24_plus;
      } // end hc1DS

      // 12-24
      n_ba.p_hiv_pop(1, s) += bf_hiv_transmission_12_24;

      // 24 plus
      n_ba.p_hiv_pop(2, s) += bf_hiv_transmission_24_plus;
    } // end NS
  }
}

template<typename ModelVariant, typename real_type>
void need_for_cotrim(int t,
                     const Parameters<ModelVariant, real_type> &pars,
                     const State<ModelVariant, real_type> &state_curr,
                     State<ModelVariant, real_type> &state_next,
                     IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "need_for_cotrim can only be called for model variants where run_child_model is true");
  constexpr auto ss_b = StateSpace<ModelVariant>().base;
  constexpr auto ss_c = StateSpace<ModelVariant>().children;
  const auto& p_hc = pars.children.children;
  const auto& p_op = pars.base.options;
  auto& n_hc = state_next.children;
  auto& i_hc = intermediate.children;

  // all children under a certain age eligible for ART
  for (int s = 0; s <ss_b.NS; ++s) {
    for (int a = 0; a < p_hc.hc_art_elig_age(t); ++a) {
      for (int cat = 0; cat < ss_c.hcTT; ++cat) {
        for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
          if (a < ss_c.hc2_agestart) {
            i_hc.hc_ctx_need(hd, cat, a, s) += n_hc.hc1_hiv_pop(hd, cat, a, s);
          } else if (hd < ss_c.hc2DS) {
            i_hc.hc_ctx_need(hd, cat, a, s) += n_hc.hc2_hiv_pop(hd, cat, a - ss_c.hc2_agestart, s);
          }
        } // end ss_c.hc1DS
      } // end a
    } // end hcTT
  } // end ss_b.NS

  // all children under a certain CD4 eligible for ART
  for (int s = 0; s <ss_b.NS; ++s) {
    for (int cat = 0; cat < ss_c.hcTT; ++cat) {
      for (int a = p_hc.hc_art_elig_age(t); a < p_op.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
          if (hd > (p_hc.hc_art_elig_cd4(a, t))) {
            if (a < ss_c.hc2_agestart) {
              i_hc.hc_ctx_need(hd, cat, a, s) += n_hc.hc1_hiv_pop(hd, cat, a, s);
            } else if (hd < ss_c.hc2DS) {
              i_hc.hc_ctx_need(hd, cat, a, s) += n_hc.hc2_hiv_pop(hd, cat, a - ss_c.hc2_agestart, s);
            }
          }
        } // end ss_c.hc1DS
      } // end a
    } // end hcTT
  } // end ss_b.NS

  // Births from the last 18 months are eligible
  n_hc.ctx_need = n_hc.hiv_births * 1.5;
  for (int s = 0; s < ss_b.NS; ++s) {
    for (int a = 1; a < p_op.p_idx_fertility_first; ++a) {
      for (int cat = 0; cat < ss_c.hcTT; ++cat) {
        for (int hd = 0; hd < ss_c.hc1DS; ++hd) {
          if (a == 1) {
            n_hc.ctx_need += n_hc.hc1_hiv_pop(hd, cat, a, s) * 0.5;
          } else if (a < ss_c.hc2_agestart) {
            n_hc.ctx_need += n_hc.hc1_hiv_pop(hd, cat, a, s);
          } else if (hd < ss_c.hc2DS) {
            n_hc.ctx_need += i_hc.hc_ctx_need(hd, cat, a, s);
          }
        }
      }
    }
  }
}

template<typename ModelVariant, typename real_type>
void cotrim_need_coverage(int t,
                          const Parameters<ModelVariant, real_type> &pars,
                          const State<ModelVariant, real_type> &state_curr,
                          State<ModelVariant, real_type> &state_next,
                          IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "cotrim_need_coverage can only be called for model variants where run_child_model is true");
  const auto& p_hc = pars.children.children;
  auto& n_hc = state_next.children;

  internal::need_for_cotrim(t, pars, state_curr, state_next, intermediate);

  if (p_hc.ctx_val_is_percent(t)) {
    n_hc.ctx_mean = p_hc.ctx_val(t - 1);
    n_hc.ctx_mean = (1 - p_hc.ctx_effect) * n_hc.ctx_mean + (1 - n_hc.ctx_mean);
  } else {
    if (n_hc.ctx_need > 0) {
      n_hc.ctx_mean = p_hc.ctx_val(t - 1) / n_hc.ctx_need;
      n_hc.ctx_mean = (1 - p_hc.ctx_effect) * n_hc.ctx_mean + (1 - n_hc.ctx_mean);
    }
  }
}

template<typename ModelVariant, typename real_type>
void run_child_natural_history(int t,
                               const Parameters<ModelVariant, real_type> &pars,
                               const State<ModelVariant, real_type> &state_curr,
                               State<ModelVariant, real_type> &state_next,
                               IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_child_natural_history can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          intermediate.children.hc_posthivmort(hd, cat, a, s) = state_next.children.hc1_hiv_pop(hd, cat, a, s) -
            state_next.children.ctx_mean * state_next.children.hc1_hiv_pop(hd, cat, a, s) * cpars.hc1_cd4_mort(hd, cat, a);

        }
      }
    }
  }

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = hc_ss.hc2_agestart; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc2DS; ++hd) {
          intermediate.children.hc_posthivmort(hd, cat, a, s) = state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) -
            state_next.children.ctx_mean * state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) * cpars.hc2_cd4_mort(hd, cat, a - hc_ss.hc2_agestart);
        }
      }
    }
  }

  //progress through CD4 categories
  for (int s = 0; s < ss.NS; ++s) {
    for (int hd = 1; hd < hc_ss.hc1DS; ++hd) {
      for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          intermediate.children.hc_grad(hd - 1, cat, a, s) -=  (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1, (cpars.hc_age_coarse_cd4(a)), s) + state_next.children.hc1_hiv_pop(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1, (cpars.hc_age_coarse_cd4(a)), s)) / 2; //moving to next cd4 category
          intermediate.children.hc_grad(hd, cat, a, s) += (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1, (cpars.hc_age_coarse_cd4(a)), s) + state_next.children.hc1_hiv_pop(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1, (cpars.hc_age_coarse_cd4(a)), s)) / 2; //moving into this cd4 category
        }
      }
    }
  }

  //progress through CD4 categories
  for (int s = 0; s < ss.NS; ++s) {
    for (int hd = 1; hd < hc_ss.hc2DS; ++hd) {
      for (int a = hc_ss.hc2_agestart; a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          intermediate.children.hc_grad(hd - 1, cat, a, s) -= (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc2_cd4_prog(hd - 1, 0, s) + state_next.children.hc2_hiv_pop(hd - 1, cat, a - hc_ss.hc2_agestart, s) * cpars.hc2_cd4_prog(hd - 1, 0, s)) / 2; //moving to next cd4 category
          intermediate.children.hc_grad(hd, cat, a, s) += (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc2_cd4_prog(hd - 1, 0, s) + state_next.children.hc2_hiv_pop(hd - 1, cat, a - hc_ss.hc2_agestart, s) * cpars.hc2_cd4_prog(hd - 1, 0, s)) / 2; //moving into this cd4 category
        }
      }
    }
  }

}

template<typename ModelVariant, typename real_type>
void run_child_hiv_mort(int t,
                        const Parameters<ModelVariant, real_type> &pars,
                        const State<ModelVariant, real_type> &state_curr,
                        State<ModelVariant, real_type> &state_next,
                        IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_child_hiv_mort can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          intermediate.children.hc_grad(hd, cat, a, s) -=  state_next.children.ctx_mean *
            state_next.children.hc1_hiv_pop(hd, cat, a, s) * cpars.hc1_cd4_mort(hd, cat, a);
          state_next.children.hc1_noart_aids_deaths(hd, cat, a, s) +=  state_next.children.ctx_mean *
            state_next.children.hc1_hiv_pop(hd, cat, a, s) * cpars.hc1_cd4_mort(hd, cat, a);
        }
      }
    }
  }

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = hc_ss.hc2_agestart; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc2DS; ++hd) {
          intermediate.children.hc_grad(hd, cat, a, s) -=  state_next.children.ctx_mean *
            state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) *
            cpars.hc2_cd4_mort(hd, cat, a - hc_ss.hc2_agestart);
          state_next.children.hc2_noart_aids_deaths(hd, cat, a - hc_ss.hc2_agestart, s) +=  state_next.children.ctx_mean *
            state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) * cpars.hc2_cd4_mort(hd, cat, a - hc_ss.hc2_agestart);
        }
      }
    }
  }

}

template<typename ModelVariant, typename real_type>
void add_child_grad(int t,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "add_child_grad can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;

  //add on transitions
  for (int s = 0; s < ss.NS; ++s) {
    for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          state_next.children.hc1_hiv_pop(hd, cat, a, s) += intermediate.children.hc_grad(hd, cat, a, s);
        }// end hc1DS
      }//end cat
    }//end a
  }// end s

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = hc_ss.hc2_agestart; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc2DS; ++hd) {
          state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) += intermediate.children.hc_grad(hd, cat, a, s);
        }//end hc2DS
      }//end cat
    }//end a
  }// end s

}

template<typename ModelVariant, typename real_type>
void hc_initiate_art_by_age(int t,
                            const Parameters<ModelVariant, real_type> &pars,
                            const State<ModelVariant, real_type> &state_curr,
                            State<ModelVariant, real_type> &state_next,
                            IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "hc_initiate_art_by_age can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  //all children under a certain age eligible for ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < cpars.hc_art_elig_age(t); ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_need_init(hd, cat, a, s) += state_next.children.hc1_hiv_pop(hd, cat, a, s);
          } else if (hd < hc_ss.hc2DS) {
            state_next.children.hc_art_need_init(hd, cat, a, s) += state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s);
          }
        } // end hc_ss.hc1DS
      } // end a
    } // end hcTT
  } // end ss.NS

}

template<typename ModelVariant, typename real_type>
void hc_initiate_art_by_cd4(int t,
                            const Parameters<ModelVariant, real_type> &pars,
                            const State<ModelVariant, real_type> &state_curr,
                            State<ModelVariant, real_type> &state_next,
                            IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "hc_initiate_art_by_cd4 can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  //all children under a certain CD4 eligible for ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
      for (int a = cpars.hc_art_elig_age(t); a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          if (hd > (cpars.hc_art_elig_cd4(a, t))) {
            if (a < hc_ss.hc2_agestart) {
              state_next.children.hc_art_need_init(hd, cat, a, s) += state_next.children.hc1_hiv_pop(hd, cat, a, s);
            } else if (hd < hc_ss.hc2DS) {
              state_next.children.hc_art_need_init(hd, cat, a, s) += state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s);
            }
          }
        } // end hc_ss.hc1DS
      } // end a
    } // end hcTT
  } // end ss.NS

}

template<typename ModelVariant, typename real_type>
void eligible_for_treatment(int t,
                            const Parameters<ModelVariant, real_type> &pars,
                            const State<ModelVariant, real_type> &state_curr,
                            State<ModelVariant, real_type> &state_next,
                            IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "eligible_for_treatment can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;

  internal::hc_initiate_art_by_age(t, pars, state_curr, state_next, intermediate);
  internal::hc_initiate_art_by_cd4(t, pars, state_curr, state_next, intermediate);

  for (int s = 0; s < ss.NS; ++s) {
    for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          intermediate.children.eligible(hd, a, s) +=  state_next.children.hc_art_need_init(hd, cat, a, s) ;
        } // end hc_ss.hc1DS
      } // end a
    } // end hcTT
  } // end ss.NS

}

template<typename ModelVariant, typename real_type>
void on_art_mortality(int t,
                     const Parameters<ModelVariant, real_type> &pars,
                     const State<ModelVariant, real_type> &state_curr,
                     State<ModelVariant, real_type> &state_next,
                     IntermediateData<ModelVariant, real_type> &intermediate,
                     int time_art_idx) {
  static_assert(ModelVariant::run_child_model,
                "on_art_mortality can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  // !!! TODO: fix order of for loop
  for (int s = 0; s <ss.NS; ++s) {
    for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        intermediate.children.hc_death_rate = 0.0;
        intermediate.children.hc_art_grad(time_art_idx, hd, a, s) = 0.0;
        if (time_art_idx == 0) {
          if (a < hc_ss.hc2_agestart) {
            intermediate.children.hc_death_rate =  cpars.hc_art_mort_rr(time_art_idx, a, t) * 0.5 * (cpars.hc1_art_mort(hd, 0, a) + cpars.hc1_art_mort(hd, 1, a));
          }else if (hd < hc_ss.hc2DS && state_next.children.hc2_art_pop(time_art_idx, hd, a-hc_ss.hc2_agestart, s) > 0) {
            intermediate.children.hc_death_rate =  cpars.hc_art_mort_rr(time_art_idx, a, t) * 0.5 * (cpars.hc2_art_mort(hd, 0, a-hc_ss.hc2_agestart) + cpars.hc2_art_mort(hd, 1, a-hc_ss.hc2_agestart));
          }
        } else {
          if (a < hc_ss.hc2_agestart) {
            intermediate.children.hc_death_rate =  cpars.hc_art_mort_rr(time_art_idx, a, t) * cpars.hc1_art_mort(hd, 2, a);
          }else if (hd < hc_ss.hc2DS && state_next.children.hc2_art_pop(time_art_idx, hd, a-hc_ss.hc2_agestart, s) > 0) {
            intermediate.children.hc_death_rate =  cpars.hc_art_mort_rr(time_art_idx, a, t) * cpars.hc2_art_mort(hd, 2, a-hc_ss.hc2_agestart);
          }
        }

        if (a < hc_ss.hc2_agestart) {
          if ((intermediate.children.hc_death_rate * state_next.children.hc1_art_pop(time_art_idx, hd, a, s)) >= 0) {
            intermediate.children.hc_art_grad(time_art_idx,hd, a, s) -= intermediate.children.hc_death_rate * state_next.children.hc1_art_pop(time_art_idx, hd, a, s);
            state_next.children.hc1_art_pop(time_art_idx, hd,  a, s) += intermediate.children.hc_art_grad(time_art_idx, hd, a, s);
            state_next.children.hc1_art_aids_deaths(time_art_idx,hd, a, s) -= intermediate.children.hc_art_grad(time_art_idx,hd, a, s);

          }
        } else if (hd < hc_ss.hc2DS) {
          if ((intermediate.children.hc_death_rate * state_next.children.hc2_art_pop(time_art_idx, hd, a-hc_ss.hc2_agestart, s)) >= 0) {
            intermediate.children.hc_art_grad(time_art_idx, hd, a, s) -= intermediate.children.hc_death_rate * state_next.children.hc2_art_pop(time_art_idx, hd, a-hc_ss.hc2_agestart, s);
            state_next.children.hc2_art_pop(time_art_idx, hd,  a-hc_ss.hc2_agestart, s) += intermediate.children.hc_art_grad(time_art_idx, hd, a, s);
            state_next.children.hc2_art_aids_deaths(time_art_idx, hd, a-hc_ss.hc2_agestart, s) -= intermediate.children.hc_art_grad(time_art_idx, hd, a, s);

          }
        }
      }// end a
    }// end hc_ss.hc1DS
  }// end ss.NS

}

template<typename ModelVariant, typename real_type>
void deaths_this_year(int t,
                      const Parameters<ModelVariant, real_type> &pars,
                      const State<ModelVariant, real_type> &state_curr,
                      State<ModelVariant, real_type> &state_next,
                      IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "deaths_this_year can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int dur = 0; dur < ss.hTS; ++dur) {
    for (int s = 0; s <ss.NS; ++s) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
          if (a < hc_ss.hc2_agestart) {
            intermediate.children.hc_art_deaths(cpars.hc_age_coarse(a)) += state_next.children.hc1_art_aids_deaths(dur,hd, a, s);
          } else {
            if (hd < hc_ss.hc2DS) {
              intermediate.children.hc_art_deaths(cpars.hc_age_coarse(a)) += state_next.children.hc2_art_aids_deaths(dur,hd, a-hc_ss.hc2_agestart, s);
            }
          }
        }// end a
      }// end hc_ss.hc1DS
    }// end ss.NS
  }// end dur
  intermediate.children.hc_art_deaths(0) =  intermediate.children.hc_art_deaths(1) +  intermediate.children.hc_art_deaths(2) + intermediate.children.hc_art_deaths(3);

}

template<typename ModelVariant, typename real_type>
void progress_time_on_art(int t,
                          const Parameters<ModelVariant, real_type> &pars,
                          const State<ModelVariant, real_type> &state_curr,
                          State<ModelVariant, real_type> &state_next,
                          IntermediateData<ModelVariant, real_type> &intermediate,
                          int current_time_idx, int end_time_idx) {
  static_assert(ModelVariant::run_child_model,
                "progress_time_on_art can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;

  //Progress ART to the correct time on ART
  for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int s = 0; s < ss.NS; ++s) {
        if (a < hc_ss.hc2_agestart) {
          if (state_next.children.hc1_art_pop(current_time_idx, hd, a, s) > 0) {
            state_next.children.hc1_art_pop(end_time_idx, hd, a, s) += state_next.children.hc1_art_pop(current_time_idx, hd, a, s);
            state_next.children.hc1_art_pop(current_time_idx, hd, a, s) -= state_next.children.hc1_art_pop(current_time_idx, hd, a, s);
          }
        } else if (hd < (hc_ss.hc2DS)) {
          state_next.children.hc2_art_pop(end_time_idx, hd, a-hc_ss.hc2_agestart, s) += state_next.children.hc2_art_pop(current_time_idx, hd, a-hc_ss.hc2_agestart, s);
          state_next.children.hc2_art_pop(current_time_idx, hd, a-hc_ss.hc2_agestart, s) -= state_next.children.hc2_art_pop(current_time_idx, hd, a-hc_ss.hc2_agestart, s);
        }
      }//end ss.NS
    }// end a
  }// end hc_ss.hc1DS

}

template<typename ModelVariant, typename real_type>
void calc_total_and_unmet_need(int t,
                               const Parameters<ModelVariant, real_type> &pars,
                               const State<ModelVariant, real_type> &state_curr,
                               State<ModelVariant, real_type> &state_next,
                               IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_total_and_unmet_need can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  internal::eligible_for_treatment(t, pars, state_curr, state_next, intermediate);

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        intermediate.children.unmet_need(cpars.hc_age_coarse(a)) += intermediate.children.eligible(hd, a, s) ;

      } // end hc_ss.hc1DS
    } // end a
  } // end ss.NS

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            intermediate.children.on_art(cpars.hc_age_coarse(a)) += state_next.children.hc1_art_pop(dur, hd, a, s);
          } else if (hd < (hc_ss.hc2DS)) {
            intermediate.children.on_art(cpars.hc_age_coarse(a)) += state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);
          }
        }
      }
    }
  }

  for (int ag = 1; ag < 4; ++ag) {
    intermediate.children.on_art(0) += intermediate.children.on_art(ag);
    intermediate.children.unmet_need(0) += intermediate.children.unmet_need(ag);
    intermediate.children.total_need(ag) += intermediate.children.on_art(ag) + intermediate.children.unmet_need(ag) + intermediate.children.hc_art_deaths(ag);

  }// end ag

  intermediate.children.total_need(0) = intermediate.children.on_art(0) + intermediate.children.unmet_need(0) + intermediate.children.hc_art_deaths(0);

}

template<typename ModelVariant, typename real_type>
void calc_age_specific_last_year(int t,
                               const Parameters<ModelVariant, real_type> &pars,
                               const State<ModelVariant, real_type> &state_curr,
                               State<ModelVariant, real_type> &state_next,
                               IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_age_specific_last_year can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  if (cpars.hc_art_is_age_spec(t-1)) {
    for (int ag = 1; ag < 4; ++ag) {
      intermediate.children.total_art_last_year(ag) =  cpars.hc_art_val(ag,t-1);
    }// end ag
  } else {
    for (int s = 0; s < ss.NS; ++s) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          for (int dur = 0; dur < ss.hTS; ++dur) {
            if (a < hc_ss.hc2_agestart) {
              intermediate.children.total_art_last_year(cpars.hc_age_coarse(a)) += state_curr.children.hc1_art_pop(dur, hd, a, s);
            } else if (hd < (hc_ss.hc2DS)) {
              intermediate.children.total_art_last_year(cpars.hc_age_coarse(a)) += state_curr.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);

            }
          }
        }
      }
    }

    intermediate.children.total_art_last_year(0) = intermediate.children.total_art_last_year(1) + intermediate.children.total_art_last_year(2) + intermediate.children.total_art_last_year(3);

    if (cpars.hc_art_isperc(t-1)){
      for (int ag = 1; ag < 4; ++ag) {
        intermediate.children.total_art_last_year(ag) =  (cpars.hc_art_val(0,t-1) * (intermediate.children.total_need(0) + intermediate.children.hc_art_deaths(ag)) * intermediate.children.total_art_last_year(ag) / intermediate.children.total_art_last_year(0)) ;
      }// end ag
    } else {
      for (int ag = 1; ag < 4; ++ag) {
        intermediate.children.total_art_last_year(ag) =  cpars.hc_art_val(0,t-1) * (intermediate.children.total_art_last_year(ag) / intermediate.children.total_art_last_year(0)) ;
      }// end ag
    }
  }

}


template<typename ModelVariant, typename real_type>
void calc_art_last_year(int t,
                        const Parameters<ModelVariant, real_type> &pars,
                        const State<ModelVariant, real_type> &state_curr,
                        State<ModelVariant, real_type> &state_next,
                        IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_art_last_year can only be called for model variants where run_child_model is true");
  const auto cpars = pars.children.children;

  if (cpars.hc_art_isperc(t-1)) { // ART entered as percent last year
    if (cpars.hc_art_is_age_spec(t)) { //If the present time step is age specific, we need to calculate what last years age spec breakdown would have been
      internal::calc_age_specific_last_year(t, pars, state_curr, state_next, intermediate);
    } else {
      intermediate.children.total_art_last_year(0) = cpars.hc_art_val(0,t-1) * (intermediate.children.total_need(0));
    }
  } else { // ART entered as number last year
    if (cpars.hc_art_is_age_spec(t)) { //If the present time step is age specific, we need to calculate what last years age spec breakdown would have been
      internal::calc_age_specific_last_year(t, pars, state_curr, state_next, intermediate);
    } else {
      if (cpars.hc_art_is_age_spec(t-1)) {
        intermediate.children.total_art_last_year(0) = cpars.hc_art_val(1,t-1) +
          cpars.hc_art_val(2,t-1) +
          cpars.hc_art_val(3,t-1);
      } else {
        intermediate.children.total_art_last_year(0) = cpars.hc_art_val(0,t-1)  ;
      }
    }
  }

}

template<typename ModelVariant, typename real_type>
void calc_art_this_year(int t,
                        const Parameters<ModelVariant, real_type> &pars,
                        const State<ModelVariant, real_type> &state_curr,
                        State<ModelVariant, real_type> &state_next,
                        IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_art_this_year can only be called for model variants where run_child_model is true");
  const auto cpars = pars.children.children;

  if (cpars.hc_art_isperc(t)) {
    for (int ag = 0; ag < 4; ++ag) {
      intermediate.children.total_art_this_year(ag) =  cpars.hc_art_val(ag,t) * (intermediate.children.total_need(ag));
    }// end ag
  } else {
    for (int ag = 0; ag < 4; ++ag) {
      intermediate.children.total_art_this_year(ag) =  cpars.hc_art_val(ag,t);
    }// end ag
  }

}

template<typename ModelVariant, typename real_type>
void calc_art_initiates(int t,
                        const Parameters<ModelVariant, real_type> &pars,
                        const State<ModelVariant, real_type> &state_curr,
                        State<ModelVariant, real_type> &state_next,
                        IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_art_initiates can only be called for model variants where run_child_model is true");
  const auto cpars = pars.children.children;

  internal::on_art_mortality(t, pars, state_curr, state_next, intermediate, 0);
  //progress art initates from 0-6 months on art to 6 to 12 mo
  internal::progress_time_on_art(t, pars, state_curr, state_next, intermediate, 0, 1);
  internal::on_art_mortality(t, pars, state_curr, state_next, intermediate, 2);
  internal::deaths_this_year(t, pars, state_curr, state_next, intermediate);
  internal::calc_total_and_unmet_need(t, pars, state_curr, state_next, intermediate);
  internal::calc_art_last_year(t, pars, state_curr, state_next, intermediate);
  internal::calc_art_this_year(t, pars, state_curr, state_next, intermediate);

  intermediate.children.retained = 1  - cpars.hc_art_ltfu(t);

  for (int ag = 0; ag < 4; ++ag) {
    state_next.children.hc_art_init(ag) = 0.5 * (intermediate.children.total_art_last_year(ag) + intermediate.children.total_art_this_year(ag)) - intermediate.children.on_art(ag) * intermediate.children.retained ;
    if (state_next.children.hc_art_init(ag) < 0) {
      state_next.children.hc_art_init(ag) = 0.0;
    }
  }// end ag

  for (int ag = 0; ag < 4; ++ag) {
    if (state_next.children.hc_art_init(ag) > (intermediate.children.unmet_need(ag)+intermediate.children.on_art(ag) * cpars.hc_art_ltfu(t)))
      state_next.children.hc_art_init(ag) = (intermediate.children.unmet_need(ag)+intermediate.children.on_art(ag) * cpars.hc_art_ltfu(t));
  }// end ag

}

template<typename ModelVariant, typename real_type>
void calc_art_ltfu(int t,
                   const Parameters<ModelVariant, real_type> &pars,
                   const State<ModelVariant, real_type> &state_curr,
                   State<ModelVariant, real_type> &state_next,
                   IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_art_ltfu can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {

        if (a < hc_ss.hc2_agestart) {
          intermediate.children.hc_hiv_total(hd, a, s) += state_next.children.hc1_hiv_pop(hd, cat, a, s);
        } else if (hd < (hc_ss.hc2DS)) {
          intermediate.children.hc_hiv_total(hd, a, s) += state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s);
        }

        } // end hc_ss.hcTT
      } //end hc_ss.hc1DS
    } // end a
  } // end ss.NS

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        if (a < hc_ss.hc2_agestart) {
          intermediate.children.hc_hiv_dist(hd, cat, a, s) =  state_next.children.hc1_hiv_pop(hd, cat, a, s) / intermediate.children.hc_hiv_total(hd, a, s);
        } else if (hd < (hc_ss.hc2DS)) {
          intermediate.children.hc_hiv_dist(hd, cat, a, s) += state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) / intermediate.children.hc_hiv_total(hd, a, s);
        }

        } // end hc_ss.hcTT
      } //end hc_ss.hc1DS
    } // end a
  } // end ss.NS

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for(int cat = 0; cat < hc_ss.hcTT; ++cat){

        if(intermediate.children.hc_hiv_total(hd, a, s)  > 0){
          if (a < hc_ss.hc2_agestart) {
            intermediate.children.art_ltfu_grad(hd, cat, a, s) +=  (state_next.children.hc1_art_pop(2, hd, a, s) + state_next.children.hc1_art_pop(0, hd, a, s)) * cpars.hc_art_ltfu(t) * intermediate.children.hc_hiv_dist(hd, cat, a, s);
          } else if (hd < (hc_ss.hc2DS)) {
            intermediate.children.art_ltfu_grad(hd, cat, a, s) +=  (state_next.children.hc2_art_pop(2, hd, a - hc_ss.hc2_agestart, s) + state_next.children.hc2_art_pop(0, hd, a - hc_ss.hc2_agestart, s)) * cpars.hc_art_ltfu(t) * intermediate.children.hc_hiv_dist(hd, cat, a, s);
          }
        }else{
          //if no one is off treatment, equally distribute across 4 transmission categories
            if (a < hc_ss.hc2_agestart) {
              intermediate.children.art_ltfu_grad(hd, cat, a, s) +=  (state_next.children.hc1_art_pop(2, hd, a, s) + state_next.children.hc1_art_pop(0, hd, a, s))  * cpars.hc_art_ltfu(t) * 0.25;
            } else if (hd < (hc_ss.hc2DS)) {
              intermediate.children.art_ltfu_grad(hd, cat, a, s) +=  (state_next.children.hc2_art_pop(2, hd, a - hc_ss.hc2_agestart, s) + state_next.children.hc2_art_pop(0, hd, a - hc_ss.hc2_agestart, s)) * cpars.hc_art_ltfu(t) * 0.25;
            }
        }

        }// end hc_ss.hcTT
      } //end hc_ss.hc1DS
    } // end a
  } // end ss.NS

}

template<typename ModelVariant, typename real_type>
void calc_art_ltfu_hivpop(int t,
                   const Parameters<ModelVariant, real_type> &pars,
                   const State<ModelVariant, real_type> &state_curr,
                   State<ModelVariant, real_type> &state_next,
                   IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_art_ltfu_hivpop can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for(int cat = 0; cat < hc_ss.hcTT; ++cat){

            if (a < hc_ss.hc2_agestart) {
              state_next.children.hc1_hiv_pop(hd, cat, a, s) += intermediate.children.art_ltfu_grad(hd, cat, a, s);
            } else if (hd < (hc_ss.hc2DS)) {
              state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) +=  intermediate.children.art_ltfu_grad(hd, cat, a, s);
            }

        }// end hc_ss.hcTT
      } //end hc_ss.hc1DS
    } // end a
  } // end ss.NS

}

template<typename ModelVariant, typename real_type>
void calc_art_ltfu_artpop(int t,
                          const Parameters<ModelVariant, real_type> &pars,
                          const State<ModelVariant, real_type> &state_curr,
                          State<ModelVariant, real_type> &state_next,
                          IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "calc_art_ltfu_artpop can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for(int cat = 0; cat < hc_ss.hcTT; ++cat){
        if (a < hc_ss.hc2_agestart) {
          //hard coded two as this will only occur among children that are on ART more than a year
          state_next.children.hc1_art_pop(2, hd, a, s) -=  intermediate.children.art_ltfu_grad(hd, cat, a, s);
        } else if (hd < (hc_ss.hc2DS )) {
          state_next.children.hc2_art_pop(2, hd, a - hc_ss.hc2_agestart, s) -= intermediate.children.art_ltfu_grad(hd, cat, a, s);
        }
        }// end hc_ss.hcTT
      } //end hc_ss.hc1DS
    } // end a
  } // end ss.NS

}

template<typename ModelVariant, typename real_type>
void hc_art_initiation_by_age(int t,
                              const Parameters<ModelVariant, real_type> &pars,
                              const State<ModelVariant, real_type> &state_curr,
                              State<ModelVariant, real_type> &state_next,
                              IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "hc_art_initiation_by_age can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  internal::calc_art_initiates(t, pars, state_curr, state_next, intermediate);

  if (cpars.hc_art_is_age_spec(t)) {
    for (int s = 0; s < ss.NS; ++s) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
            intermediate.children.hc_initByAge(cpars.hc_age_coarse(a)) += state_next.children.hc_art_need_init(hd, cat, a, s) * cpars.hc_art_init_dist(a, t);
          } //end hcTT
        } // end hc_ss.hc1DS
      } //end a
    } // end ss.NS

    for (int ag = 1; ag < 4; ++ag) {
      if (intermediate.children.hc_initByAge(ag) == 0.0) {
        intermediate.children.hc_adj(ag) = 1.0 ;
      } else {
        intermediate.children.hc_adj(ag) =  state_next.children.hc_art_init(ag) / intermediate.children.hc_initByAge(ag);
      }
    }

    for (int s = 0; s <ss.NS; ++s) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
          for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
            if ((intermediate.children.hc_adj(cpars.hc_age_coarse(a)) * cpars.hc_art_init_dist(a, t)) > 1.0) {
              intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) = 1.0;
            } else {
              intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) = intermediate.children.hc_adj(cpars.hc_age_coarse(a)) * cpars.hc_art_init_dist(a, t);
            }
            if ((cpars.hc_art_val(0, t) + cpars.hc_art_val(1, t) + cpars.hc_art_val(2, t) + cpars.hc_art_val(3, t)) > 0.0) {
              intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) = intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a));
            } else {
              intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) = 0.0;
            }


            if (a < hc_ss.hc2_agestart) {
              state_next.children.hc1_art_pop(0, hd, a, s) += intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) * state_next.children.hc_art_need_init(hd, cat, a, s);
            } else if (hd < (hc_ss.hc2DS)) {
              state_next.children.hc2_art_pop(0, hd, a - hc_ss.hc2_agestart, s) += intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) * state_next.children.hc_art_need_init(hd, cat, a, s);
            }
            if (a < hc_ss.hc2_agestart) {
              state_next.children.hc1_hiv_pop(hd, cat, a, s) -= intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) * state_next.children.hc_art_need_init(hd, cat, a, s);
            } else if (hd < (hc_ss.hc2DS )) {
              state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) -=  intermediate.children.hc_art_scalar(cpars.hc_age_coarse(a)) * state_next.children.hc_art_need_init(hd, cat, a, s);
            }

          } //end hc_ss.hc1DS
        } // end a
      } // end hcTT
    } // end ss.NS
  } else {
    for (int s = 0; s <ss.NS; ++s) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
            intermediate.children.hc_initByAge(0)  += state_next.children.hc_art_need_init(hd, cat, a, s) * cpars.hc_art_init_dist(a, t);
          } //end hcTT
        } // end hc_ss.hc1DS
      } //end a
    } // end ss.NS

    if (intermediate.children.hc_initByAge(0) == 0.0) {
      intermediate.children.hc_adj(0) = 1.0 ;
    } else {
      intermediate.children.hc_adj(0) =  state_next.children.hc_art_init(0) / intermediate.children.hc_initByAge(0);
    }

    for (int s = 0; s <ss.NS; ++s) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
          for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {


            if ((intermediate.children.hc_adj(0) * cpars.hc_art_init_dist(a, t)) > 1.0) {
              intermediate.children.hc_art_scalar(0) = 1.0;
              //issue is that in 2030 this is being activated when it shouldn't be for age one
              //TLDR is that too many people are initiating who shouldn't bev
            } else {
              intermediate.children.hc_art_scalar(0) = intermediate.children.hc_adj(0) * cpars.hc_art_init_dist(a, t);
            }

            if ((cpars.hc_art_val(0, t) + cpars.hc_art_val(0, t - 1)) > 0.0) {
              intermediate.children.hc_art_scalar(0) = intermediate.children.hc_art_scalar(0);
            } else {
              intermediate.children.hc_art_scalar(0) = 0.0;
            }

            if (a < hc_ss.hc2_agestart) {
              state_next.children.hc1_art_pop(0, hd, a, s) += intermediate.children.hc_art_scalar(0) * state_next.children.hc_art_need_init(hd, cat, a, s);
            } else if (hd < (hc_ss.hc2DS)) {
              state_next.children.hc2_art_pop(0, hd, a - hc_ss.hc2_agestart, s) += intermediate.children.hc_art_scalar(0) * state_next.children.hc_art_need_init(hd, cat, a, s);
            }

            if (a < hc_ss.hc2_agestart) {
              state_next.children.hc1_hiv_pop(hd, cat, a, s) -= intermediate.children.hc_art_scalar(0) * state_next.children.hc_art_need_init(hd, cat, a, s);
            } else if (hd < (hc_ss.hc2DS )) {
              state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) -=  intermediate.children.hc_art_scalar(0) * state_next.children.hc_art_need_init(hd, cat, a, s);
            }
          } //end hc_ss.hc1DS
        } // end a
      } // end hcTT
    } // end ss.NS
  }// end if

}

template<typename ModelVariant, typename real_type>
void fill_model_outputs(int t,
                        const Parameters<ModelVariant, real_type> &pars,
                        const State<ModelVariant, real_type> &state_curr,
                        State<ModelVariant, real_type> &state_next,
                        IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "fill_model_outputs can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;

  for (int hd = 0; hd < ss.hDS; ++hd) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int s = 0; s < ss.NS; ++s) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          if (a < hc_ss.hc2_agestart) {
            state_next.base.p_hiv_deaths(a,s) += state_next.children.hc1_noart_aids_deaths(hd, cat, a, s);
          } else if (hd < (hc_ss.hc2DS)) {
            state_next.base.p_hiv_deaths(a,s) += state_next.children.hc2_noart_aids_deaths(hd, cat, a-hc_ss.hc2_agestart, s);
          }
        }// end hcTT

        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart && state_next.children.hc1_art_pop(0, hd, a, s) > 0) {
            state_next.base.p_hiv_deaths(a,s) += state_next.children.hc1_art_aids_deaths(dur, hd, a, s);
          } else if (hd < (hc_ss.hc2DS) && a >= hc_ss.hc2_agestart) {
            state_next.base.p_hiv_deaths(a,s) += state_next.children.hc2_art_aids_deaths(dur, hd, a-hc_ss.hc2_agestart, s);
          }
        }//end dur
      }//end ss.NS
    }// end a
  }// end hDS

}

}// namespace internal

template<typename ModelVariant, typename real_type>
void run_child_model_simulation(int t,
                                const Parameters<ModelVariant, real_type> &pars,
                                const State<ModelVariant, real_type> &state_curr,
                                State<ModelVariant, real_type> &state_next,
                                internal::IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto cpars = pars.children.children;
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;

  internal::run_child_ageing(t, pars, state_curr, state_next, intermediate);

    if (cpars.mat_prev_input(t)) {
      internal::run_wlhiv_births_input_mat_prev(t, pars, state_curr, state_next, intermediate);
    } else {
      internal::run_wlhiv_births(t, pars, state_curr, state_next, intermediate);
    }

    internal::adjust_hiv_births(t, pars, state_curr, state_next, intermediate);

    // if (state_next.children.hiv_births > 0) {
    internal::add_infections(t, pars, state_curr, state_next, intermediate);
    internal::cotrim_need_coverage(t, pars, state_curr, state_next, intermediate);
    internal::run_child_natural_history(t, pars, state_curr, state_next, intermediate);
    internal::run_child_hiv_mort(t, pars, state_curr, state_next, intermediate);
    internal::add_child_grad(t, pars, state_curr, state_next, intermediate);

      //assume paed art doesn't start before adult
      if (t > pars.base.options.ts_art_start) {
        internal::calc_art_ltfu(t, pars, state_curr, state_next, intermediate);
        internal::hc_art_initiation_by_age(t, pars, state_curr, state_next, intermediate);
        //mortality among those on ART less than one year
        internal::on_art_mortality(t, pars, state_curr, state_next, intermediate, 0);
        internal::progress_time_on_art(t, pars, state_curr, state_next, intermediate, 1, 2);
        //progress 6 to 12 mo to 12 plus months#
        internal::calc_art_ltfu_hivpop(t, pars, state_curr, state_next, intermediate);
        internal::calc_art_ltfu_artpop(t, pars, state_curr, state_next, intermediate);
        internal::fill_model_outputs(t, pars, state_curr, state_next, intermediate);
        internal::nosocomial_infections(t, pars, state_curr, state_next, intermediate);
      }

    }

} // namespace leapfrog

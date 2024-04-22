#pragma once

#include "types.hpp"

namespace leapfrog {

namespace internal {

template<typename ModelVariant, typename real_type>
void run_child_ageing(int time_step,
                      const Parameters<ModelVariant, real_type> &pars,
                      const State<ModelVariant, real_type> &state_curr,
                      State<ModelVariant, real_type> &state_next,
                      IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto demog = pars.base.demography;
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;

  for (int s = 0; s < ss.NS; ++s) {
    //less than 5 because there is a cd4 transition between ages 4 and 5
    for (int a = 1; a < hc_ss.hc2_agestart; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          state_next.children.hc1_hiv_pop(hd, cat, a, s) +=
            state_curr.children.hc1_hiv_pop(hd, cat, a - 1, s) * demog.survival_probability(a, s, time_step);
        }
        for (int dur = 0; dur < ss.hTS; ++dur) {
          state_next.children.hc1_art_pop(dur, hd, a, s) +=
            state_curr.children.hc1_art_pop(dur, hd, a - 1, s) * demog.survival_probability(a, s, time_step);
        }
      }
    }
  }

  for (int s = 0; s < ss.NS; ++s) {
    for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
      for (int hd_alt = 0; hd_alt < hc_ss.hc2DS; ++hd_alt) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          state_next.children.hc2_hiv_pop(hd_alt, cat, 0, s) += state_curr.children.hc1_hiv_pop(hd, cat, hc_ss.hc1_ageend, s) * demog.survival_probability(hc_ss.hc2_agestart, s, time_step) * cpars.hc_cd4_transition(hd_alt, hd);
        }
        for (int dur = 0; dur < ss.hTS; ++dur) {
          state_next.children.hc2_art_pop(dur, hd_alt, 0, s) += state_curr.children.hc1_art_pop(dur, hd, hc_ss.hc1_ageend, s) * demog.survival_probability(hc_ss.hc2_agestart, s, time_step) * cpars.hc_cd4_transition(hd_alt, hd);
        }
      }
    }
  }

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = (hc_ss.hc2_agestart + 1); a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc2DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) +=
            state_curr.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart - 1, s) *
            demog.survival_probability(a, s, time_step);
        }
        for (int dur = 0; dur < ss.hTS; ++dur) {
          state_next.children.hc2_art_pop(dur, hd, a - hc_ss.hc2_agestart, s) +=
            state_curr.children.hc2_art_pop(dur, hd, a - hc_ss.hc2_agestart - 1, s) *
            demog.survival_probability(a, s, time_step);
        }
      }
    }
  }
}

template<typename ModelVariant, typename real_type>
void calc_hiv_negative_pop(int time_step,
                           const Parameters<ModelVariant, real_type> &pars,
                           const State<ModelVariant, real_type> &state_curr,
                           State<ModelVariant, real_type> &state_next,
                           IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto demog = pars.base.demography;
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;


  for(int s = 0; s < ss.NS; ++s){
    for(int a = 0; a < ss.hAG; ++a){
      intermediate.children.p_hiv_neg_pop(a, s) = demog.base_pop(a, s) - state_next.base.p_hiv_pop(a, s);
    }// end a
  }//end s

}

template<typename ModelVariant, typename real_type>
void convert_PMTCT_num_to_perc(int time_step,
                               const Parameters<ModelVariant, real_type> &pars,
                               const State<ModelVariant, real_type> &state_curr,
                               State<ModelVariant, real_type> &state_next,
                               IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;


  for (int hp = 0; hp < hc_ss.hPS; ++hp) {
    intermediate.children.sumARV += cpars.PMTCT(hp,time_step);
  }
  intermediate.children.need_PMTCT = state_next.children.hiv_births;

  //replace all instances of coverage input as numbers with percentage covered
  if(cpars.PMTCT_input_is_percent(time_step)){
    for (int hp = 0; hp < hc_ss.hPS; ++hp) {
        intermediate.children.PMTCT_coverage(hp) = cpars.PMTCT(hp,time_step) / 100;
    } //end hPS
  }else{
    for (int hp = 0; hp < hc_ss.hPS; ++hp) {
      if(intermediate.children.sumARV > intermediate.children.need_PMTCT){
        intermediate.children.PMTCT_coverage(hp) = cpars.PMTCT(hp, time_step) / intermediate.children.sumARV;
      }else{
        if(intermediate.children.need_PMTCT > 0){
          intermediate.children.PMTCT_coverage(hp) = cpars.PMTCT(hp,time_step) /  intermediate.children.need_PMTCT;
        }else{
          intermediate.children.PMTCT_coverage(hp) = 0;
        }
      }
    } //end hPS
  }

}

template<typename ModelVariant, typename real_type>
void calc_wlhiv_cd4_proportion(int time_step,
                                    const Parameters<ModelVariant, real_type> &pars,
                                    const State<ModelVariant, real_type> &state_curr,
                                    State<ModelVariant, real_type> &state_next,
                                    IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");

  //Option A and B were only authorized for women with greater than 350 CD4, so if the percentage of women
  //on option A/B > the proportion of women in this cd4 category, we assume that some must have a cd4 less than 350
  //option AB will be less effective for these women so we adjust for that
  intermediate.children.num_wlhiv_lt200 = 0.0;
  intermediate.children.num_wlhiv_200to350 = 0.0;
  intermediate.children.num_wlhiv_gte350 = 0.0;
  intermediate.children.num_wlhiv = 0.0;
  intermediate.children.prop_wlhiv_lt200 = 0.0;
  intermediate.children.prop_wlhiv_200to350 = 0.0;
  intermediate.children.prop_wlhiv_gte350 = 0.0;
  intermediate.children.prop_wlhiv_lt350 = 0.0;

  for (int a = 0; a < 35; ++a) {
    intermediate.children.num_wlhiv_lt200 += state_next.base.h_hiv_adult(4,a,1) + state_next.base.h_hiv_adult(5,a,1) + state_next.base.h_hiv_adult(6,a,1) ;
    intermediate.children.num_wlhiv_200to350 += state_next.base.h_hiv_adult(3,a,1) + state_next.base.h_hiv_adult(2,a,1) ;
    intermediate.children.num_wlhiv_gte350 += state_next.base.h_hiv_adult(0,a,1) + state_next.base.h_hiv_adult(1,a,1) ;
  }

  intermediate.children.num_wlhiv = intermediate.children.num_wlhiv_200to350 + intermediate.children.num_wlhiv_gte350 + intermediate.children.num_wlhiv_lt200;


  if (intermediate.children.num_wlhiv >0) {
    intermediate.children.prop_wlhiv_lt200 = intermediate.children.num_wlhiv_lt200/ intermediate.children.num_wlhiv;
    intermediate.children.prop_wlhiv_200to350 = intermediate.children.num_wlhiv_200to350 / intermediate.children.num_wlhiv;
    intermediate.children.prop_wlhiv_gte350 = intermediate.children.num_wlhiv_gte350 / intermediate.children.num_wlhiv;
  }else{
    intermediate.children.prop_wlhiv_lt200 = 0;
    intermediate.children.prop_wlhiv_200to350 = 1;
    intermediate.children.prop_wlhiv_gte350 = 0;
  }

  intermediate.children.prop_wlhiv_lt350 = intermediate.children.prop_wlhiv_lt200 + intermediate.children.prop_wlhiv_200to350;

}


template<typename ModelVariant, typename real_type>
void adjust_optAB_transmission_rate(int time_step,
                                    const Parameters<ModelVariant, real_type> &pars,
                                    const State<ModelVariant, real_type> &state_curr,
                                    State<ModelVariant, real_type> &state_next,
                                    IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");

  //Option A and B were only authorized for women with greater than 350 CD4, so if the percentage of women
  //on option A/B > the proportion of women in this cd4 category, we assume that some must have a cd4 less than 350
  //option AB will be less effective for these women so we adjust for that
  internal::calc_wlhiv_cd4_proportion(time_step, pars, state_curr, state_next, intermediate);


  if ((intermediate.children.PMTCT_coverage(0) + intermediate.children.PMTCT_coverage(1)) > intermediate.children.prop_wlhiv_gte350) {
    if (intermediate.children.prop_wlhiv_gte350 > 0) {
      intermediate.children.excessratio = ((intermediate.children.PMTCT_coverage(0) + intermediate.children.PMTCT_coverage(1)) / intermediate.children.prop_wlhiv_gte350) - 1;
    }else{
      intermediate.children.excessratio = 0;
    }
    intermediate.children.optA_transmission_rate = cpars.PMTCT_transmission_rate(0,0,0) * (1 + intermediate.children.excessratio);
    intermediate.children.optB_transmission_rate = cpars.PMTCT_transmission_rate(0,1,0) * (1 + intermediate.children.excessratio);
  }
  else{
    intermediate.children.excessratio = 0.0;
    intermediate.children.optA_transmission_rate = cpars.PMTCT_transmission_rate(0,0,0) * (1 + intermediate.children.excessratio);
    intermediate.children.optB_transmission_rate = cpars.PMTCT_transmission_rate(0,1,0) * (1 + intermediate.children.excessratio);
  }
}

template<typename ModelVariant, typename real_type>
void adjust_optAB_bf_transmission_rate(int time_step,
                                       const Parameters<ModelVariant, real_type> &pars,
                                       const State<ModelVariant, real_type> &state_curr,
                                       State<ModelVariant, real_type> &state_next,
                                       IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");

  internal::calc_wlhiv_cd4_proportion(time_step, pars, state_curr, state_next, intermediate);

  //Option A and B were only authorized for women with greater than 350 CD4, so if the percentage of women
  //on option A/B > the proportion of women in this cd4 category, we assume that some must have a cd4 less than 350
  //option AB will be less effective for these women so we adjust for that


  if(intermediate.children.prop_wlhiv_gte350 > 0){
    //CHECK: defined in previous function
    //CHECK: avenir: will PMTCT coverage for pregnatn and bf women always be the same?
    if((intermediate.children.PMTCT_coverage(0) + intermediate.children.PMTCT_coverage(1)) > intermediate.children.prop_wlhiv_gte350){
      intermediate.children.excessratio_bf = intermediate.children.PMTCT_coverage(0) + intermediate.children.PMTCT_coverage(1) - intermediate.children.prop_wlhiv_gte350;
      intermediate.children.optA_bf_transmission_rate = (intermediate.children.prop_wlhiv_gte350 * cpars.PMTCT_transmission_rate(4,0,1)) + intermediate.children.excessratio_bf * (1.45 / 0.46) * cpars.PMTCT_transmission_rate(4,0,1) / (intermediate.children.prop_wlhiv_gte350 + intermediate.children.excessratio_bf);
      intermediate.children.optB_bf_transmission_rate = (intermediate.children.prop_wlhiv_gte350 * cpars.PMTCT_transmission_rate(4,1,1)) + intermediate.children.excessratio_bf * (1.45 / 0.46) * cpars.PMTCT_transmission_rate(4,1,1) / (intermediate.children.prop_wlhiv_gte350 + intermediate.children.excessratio_bf);
    }else{
      intermediate.children.optA_bf_transmission_rate = cpars.PMTCT_transmission_rate(4,0,1);
      intermediate.children.optB_bf_transmission_rate = cpars.PMTCT_transmission_rate(4,1,1);
    }
  }else{
    intermediate.children.optA_bf_transmission_rate = cpars.PMTCT_transmission_rate(4,0,1);
    intermediate.children.optB_bf_transmission_rate = cpars.PMTCT_transmission_rate(4,1,1);
  }


}


template<typename ModelVariant, typename real_type>
void run_calculate_perinatal_transmission_rate(int time_step,
                                               const Parameters<ModelVariant, real_type> &pars,
                                               const State<ModelVariant, real_type> &state_curr,
                                               State<ModelVariant, real_type> &state_next,
                                               IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto demog = pars.base.demography;
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");

  //TODO: add in patients reallocated
  //TODO: pull incidence infection mtct into the input object

  internal::convert_PMTCT_num_to_perc(time_step, pars, state_curr, state_next, intermediate);
  internal::adjust_optAB_transmission_rate(time_step, pars, state_curr, state_next, intermediate);

  // ///////////////////////////////////
  // //Calculate transmission rate
  // ///////////////////////////////////
  intermediate.children.retained_on_ART = intermediate.children.PMTCT_coverage(4) * cpars.PMTCT_dropout(0,time_step) / 100;
  intermediate.children.retained_started_ART = intermediate.children.PMTCT_coverage(5) * cpars.PMTCT_dropout(1,time_step) / 100;
  //Transmission among women on treatment
  intermediate.children.perinatal_transmission_rate = intermediate.children.PMTCT_coverage(2) * cpars.PMTCT_transmission_rate(0,2,0) + //SDNVP
    intermediate.children.PMTCT_coverage(3) * cpars.PMTCT_transmission_rate(0,3,0) + //dual ARV
    intermediate.children.PMTCT_coverage(0) * intermediate.children.optA_transmission_rate +
    intermediate.children.PMTCT_coverage(1) * intermediate.children.optB_transmission_rate +
    intermediate.children.retained_on_ART * cpars.PMTCT_transmission_rate(0,4,0) +
    intermediate.children.retained_started_ART * cpars.PMTCT_transmission_rate(0,5,0) +
    intermediate.children.PMTCT_coverage(6) * cpars.PMTCT_transmission_rate(0,6,0);


  intermediate.children.receiving_PMTCT = intermediate.children.PMTCT_coverage(0) + intermediate.children.PMTCT_coverage(1) + intermediate.children.PMTCT_coverage(2) + intermediate.children.PMTCT_coverage(3) + intermediate.children.retained_on_ART + intermediate.children.retained_started_ART + intermediate.children.PMTCT_coverage(6);
  intermediate.children.no_PMTCT = 1 - intermediate.children.receiving_PMTCT;

  //Transmission among women not on treatment
  if (intermediate.children.num_wlhiv > 0) {
    intermediate.children.perinatal_transmission_rate = intermediate.children.perinatal_transmission_rate +
      intermediate.children.no_PMTCT *
      (intermediate.children.prop_wlhiv_lt200 * cpars.vertical_transmission_rate(4,0) +
      intermediate.children.prop_wlhiv_200to350 * cpars.vertical_transmission_rate(2,0) +
      intermediate.children.prop_wlhiv_gte350 * cpars.vertical_transmission_rate(0,0));
  }else{
    intermediate.children.perinatal_transmission_rate = intermediate.children.perinatal_transmission_rate;
  }
  intermediate.children.perinatal_transmission_rate_bf_calc = intermediate.children.perinatal_transmission_rate;

  //Transmission due to incident infections
  intermediate.children.asfr_sum = 0.0;
  for (int a = 0; a < pars.base.options.p_fertility_age_groups; ++a) {
    intermediate.children.asfr_sum += demog.age_specific_fertility_rate(a, time_step);
  }// end a
  for (int a = 0; a < pars.base.options.p_fertility_age_groups; ++a) {
    intermediate.children.age_weighted_hivneg += demog.age_specific_fertility_rate(a, time_step) / intermediate.children.asfr_sum  * intermediate.children.p_hiv_neg_pop(a + 15,1) ; //HIV negative 15-49 women weighted for ASFR
    intermediate.children.age_weighted_infections +=  demog.age_specific_fertility_rate(a, time_step) / intermediate.children.asfr_sum  * state_curr.base.p_infections(a + 15,1) ; //newly infected 15-49 women, weighted for ASFR
  }//end

if(intermediate.children.age_weighted_hivneg > 0.0){
  intermediate.children.incidence_rate_wlhiv = intermediate.children.age_weighted_infections / intermediate.children.age_weighted_hivneg;
  intermediate.children.perinatal_transmission_from_incidence = intermediate.children.incidence_rate_wlhiv * (9/12) * (intermediate.children.births_sum - intermediate.children.need_PMTCT) * 0;  //.181;
}else{
  intermediate.children.incidence_rate_wlhiv = 0.0;
  intermediate.children.perinatal_transmission_from_incidence = 0.0;
}

  if (intermediate.children.need_PMTCT > 0.0) {
    intermediate.children.perinatal_transmission_rate = intermediate.children.perinatal_transmission_rate + intermediate.children.perinatal_transmission_from_incidence / intermediate.children.need_PMTCT;
  }else{
    intermediate.children.perinatal_transmission_rate = intermediate.children.perinatal_transmission_rate;
  }


}

template<typename ModelVariant, typename real_type>
void run_calculate_transmission_from_incidence_during_breastfeeding(int time_step,
                                                                    const Parameters<ModelVariant, real_type> &pars,
                                                                    const State<ModelVariant, real_type> &state_curr,
                                                                    State<ModelVariant, real_type> &state_next,
                                                                    IntermediateData<ModelVariant, real_type> &intermediate) {
  const auto cpars = pars.children.children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;


  //bf_duration is % of women no longer breast feeding
  for (int bf = 0; bf < hc_ss.hBF; ++bf) {
    //CHECK: intermediate.children.incidence_rate_wlhiv is coming from perinatal transmission, are there
    // any checks I should do to ensure that this is reading in properly from the previous function?
    intermediate.children.bf_at_risk += intermediate.children.incidence_rate_wlhiv / 12 * 2 * (1 - cpars.breastfeeding_duration_no_art(bf, time_step));
  }
  // intermediate.children.bf_incident_hiv_transmission_rate = bf_at_risk * 0.269;
  intermediate.children.bf_incident_hiv_transmission_rate = 0.0;

}

template<typename ModelVariant, typename real_type>
void run_bf_transmission_rate(int time_step,
                              const Parameters<ModelVariant, real_type> &pars,
                              IntermediateData<ModelVariant, real_type> &intermediate,
                              int bf_start, int bf_end, int index) {
  const auto cpars = pars.children.children;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");

  intermediate.children.percent_no_treatment = 1.0;
  for(int bf = bf_start; bf < bf_end; bf++){
    //intermediate.children.perinatal_transmission_rate_bf_calc is the transmission that has already occurred due to perinatal transmission
    //intermediate.children.percent_no_treatment is the percentage of women who are still vulnerable to HIV transmission to their babies
    intermediate.children.percent_no_treatment = 1 - intermediate.children.perinatal_transmission_rate_bf_calc - intermediate.children.bf_transmission_rate(index);


      for(int hp = 0; hp < hc_ss.hPS; hp++){
        //hp = 0 is option A
        //Dropout not used for option A
        if(hp == 0){
          intermediate.children.percent_on_treatment = intermediate.children.optA_bf_transmission_rate *
            intermediate.children.PMTCT_coverage(hp);
          intermediate.children.bf_transmission_rate(index) += intermediate.children.percent_on_treatment *
            2 * (1 - cpars.breastfeeding_duration_art(bf, time_step)) ;

          intermediate.children.percent_no_treatment -= intermediate.children.PMTCT_coverage(hp);

        }
        //hp = 1 is option B
        //Dropout not used for option B
        if(hp == 1){
          intermediate.children.percent_on_treatment = intermediate.children.optB_bf_transmission_rate *
            intermediate.children.PMTCT_coverage(hp);
          intermediate.children.bf_transmission_rate(index) +=  intermediate.children.percent_on_treatment *
            2 * (1 - cpars.breastfeeding_duration_art(bf, time_step)) ;
          intermediate.children.percent_no_treatment -=  intermediate.children.PMTCT_coverage(hp) ;

        }
        if(hp > 3){
          if(bf > 0){
            intermediate.children.percent_on_treatment = intermediate.children.PMTCT_coverage(hp) *
              (pow(1 - cpars.PMTCT_dropout(4,time_step) / 100 * 2, bf))  ;

          }else{
            intermediate.children.percent_on_treatment = intermediate.children.PMTCT_coverage(hp);
          }


          intermediate.children.bf_transmission_rate(index) += intermediate.children.percent_on_treatment *
            2 * (1 - cpars.breastfeeding_duration_art(bf, time_step)) * cpars.PMTCT_transmission_rate(4,hp,1);

          intermediate.children.percent_no_treatment -= intermediate.children.percent_on_treatment ;
        }
      }

    if(intermediate.children.percent_no_treatment < 0){
      intermediate.children.percent_no_treatment = 0;
    }

    //No treatment
    if(cpars.breastfeeding_duration_no_art(bf, time_step) < 1){
      if(intermediate.children.optB_bf_transmission_rate > 0){
        intermediate.children.bf_transmission_rate(index) +=
          intermediate.children.percent_no_treatment * (1 - cpars.breastfeeding_duration_no_art(bf, time_step)) *
          (2 * (1 - intermediate.children.prop_wlhiv_gte350) * cpars.vertical_transmission_rate(2,1) +
          2 * intermediate.children.prop_wlhiv_gte350 * cpars.vertical_transmission_rate(0,1));
      }
    }

    if(bf < 1){
      intermediate.children.bf_transmission_rate(index) = intermediate.children.bf_transmission_rate(index)/ 4;
    }

  }



}


template<typename ModelVariant, typename real_type>
void run_child_hiv_infections(int time_step,
                              const Parameters<ModelVariant, real_type> &pars,
                              const State<ModelVariant, real_type> &state_curr,
                              State<ModelVariant, real_type> &state_next,
                              IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  const auto demog = pars.base.demography;
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s < ss.NS; ++s) {
    // Run only first 5 age groups in total population 0, 1, 2, 3, 4
    for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
      if (cpars.hc_nosocomial(time_step) > 0) {
        //5.0 is used because we want to evenly distribute across the 5 age groups in 0-4
        state_next.base.p_infections(a, s) = cpars.hc_nosocomial(time_step) / (5.0 * ss.NS);
        state_next.base.p_hiv_pop(a, s) += state_next.base.p_infections(a, s);

        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          // putting them all in perinatal hcTT to match spec nosocomial
          if (cpars.hc1_cd4_dist(hd) > 0) {
            state_next.children.hc1_hiv_pop(hd, 0, a, s) += state_next.base.p_infections(a, s) * cpars.hc1_cd4_dist(hd);
          }
        } // end hc1DS
      }
    } //end a
  } // end NS

  internal::run_calculate_perinatal_transmission_rate(time_step, pars, state_curr, state_next, intermediate);

  //Perinatal transmission
  for (int s = 0; s < ss.NS; ++s) {
  for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
       state_next.children.hc1_hiv_pop(hd, 0, 0, s) +=  state_next.children.hiv_births * intermediate.children.perinatal_transmission_rate * demog.births_sex_prop(s,time_step) * cpars.hc1_cd4_dist(hd);
      }// end hc1DS
      state_next.base.p_hiv_pop(0, s) +=  state_next.children.hiv_births * intermediate.children.perinatal_transmission_rate * demog.births_sex_prop(s,time_step);
      state_next.base.p_infections(0, s) += state_next.children.hiv_births * intermediate.children.perinatal_transmission_rate * demog.births_sex_prop(s,time_step);

  }// end NS


   //Breastfeeding transmission

   //0-6

   internal::adjust_optAB_bf_transmission_rate(time_step, pars, state_curr, state_next, intermediate);
    internal::run_calculate_transmission_from_incidence_during_breastfeeding(time_step, pars, state_curr, state_next, intermediate);
    internal::run_bf_transmission_rate(time_step, pars, intermediate, 0, 3, 0);

  for (int s = 0; s < ss.NS; ++s) {
     for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
       state_next.children.hc1_hiv_pop(hd, 1, 0, s) +=  state_next.children.hiv_births *  demog.births_sex_prop(s,time_step) * cpars.hc1_cd4_dist(hd) * (intermediate.children.bf_incident_hiv_transmission_rate + intermediate.children.bf_transmission_rate(0));
     }// end hc1DS
     state_next.base.p_hiv_pop(0, s) +=  state_next.children.hiv_births  * demog.births_sex_prop(s,time_step) * (intermediate.children.bf_incident_hiv_transmission_rate + intermediate.children.bf_transmission_rate(0));
     state_next.base.p_infections(0, s) += state_next.children.hiv_births  * demog.births_sex_prop(s,time_step) * (intermediate.children.bf_incident_hiv_transmission_rate + intermediate.children.bf_transmission_rate(0));
   }// end NS

   //6-12
   internal::run_bf_transmission_rate(time_step, pars, intermediate, 3, 6, 1);
   for (int s = 0; s < ss.NS; ++s) {
     for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
       state_next.children.hc1_hiv_pop(hd, 2, 0, s) +=  state_next.children.hiv_births *  demog.births_sex_prop(s,time_step) * cpars.hc1_cd4_dist(hd) * intermediate.children.bf_transmission_rate(1);
     }// end hc1DS
     state_next.base.p_hiv_pop(0, s) +=  state_next.children.hiv_births  * demog.births_sex_prop(s,time_step) * (intermediate.children.bf_transmission_rate(1));
     state_next.base.p_infections(0, s) += state_next.children.hiv_births  * demog.births_sex_prop(s,time_step) * (intermediate.children.bf_transmission_rate(1));
   }// end NS

   //12plus
   internal::run_bf_transmission_rate(time_step, pars, intermediate, 6, 12, 2);
   internal::run_bf_transmission_rate(time_step, pars, intermediate, 12, hc_ss.hBF, 3);
   for (int s = 0; s < ss.NS; ++s) {
     for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
       //12-24
       state_next.children.hc1_hiv_pop(hd, 3, 1, s) +=  state_next.children.hiv_births * cpars.hc1_cd4_dist(hd) * intermediate.children.bf_transmission_rate(2) *
         (state_next.base.p_total_pop(1,s) - state_next.base.p_hiv_pop(1,s)) / ((state_next.base.p_total_pop(1,0) + state_next.base.p_total_pop(1,1)) - (state_next.base.p_hiv_pop(1,0) + state_next.base.p_hiv_pop(1,1)));
       //24 plus
       state_next.children.hc1_hiv_pop(hd, 3, 2, s) +=  state_next.children.hiv_births  * cpars.hc1_cd4_dist(hd) * intermediate.children.bf_transmission_rate(3) *
         (state_next.base.p_total_pop(2,s) - state_next.base.p_hiv_pop(2,s)) / ((state_next.base.p_total_pop(2,0) + state_next.base.p_total_pop(2,1)) - (state_next.base.p_hiv_pop(2,0) + state_next.base.p_hiv_pop(2,1)));
     }// end hc1DS
     //12-24
     state_next.base.p_hiv_pop(1, s) +=  state_next.children.hiv_births * (intermediate.children.bf_transmission_rate(2)) *
       (state_next.base.p_total_pop(1,s) - state_next.base.p_hiv_pop(1,s)) / ((state_next.base.p_total_pop(1,0) + state_next.base.p_total_pop(1,1)) - (state_next.base.p_hiv_pop(1,0) + state_next.base.p_hiv_pop(1,1)));
     state_next.base.p_infections(1, s) += state_next.children.hiv_births  * demog.births_sex_prop(s,time_step) * (intermediate.children.bf_transmission_rate(2)) *
       (state_next.base.p_total_pop(1,s) - state_next.base.p_hiv_pop(1,s)) / ((state_next.base.p_total_pop(1,0) + state_next.base.p_total_pop(1,1)) - (state_next.base.p_hiv_pop(1,0) + state_next.base.p_hiv_pop(1,1)));
     //24 plus
     state_next.base.p_hiv_pop(2, s) +=  state_next.children.hiv_births  * (intermediate.children.bf_transmission_rate(3))*
       (state_next.base.p_total_pop(2,s) - state_next.base.p_hiv_pop(2,s)) / ((state_next.base.p_total_pop(2,0) + state_next.base.p_total_pop(2,1)) - (state_next.base.p_hiv_pop(2,0) + state_next.base.p_hiv_pop(2,1)));
     state_next.base.p_infections(2, s) += state_next.children.hiv_births * (intermediate.children.bf_transmission_rate(3))*
       (state_next.base.p_total_pop(2,s) - state_next.base.p_hiv_pop(2,s)) / ((state_next.base.p_total_pop(2,0) + state_next.base.p_total_pop(2,1)) - (state_next.base.p_hiv_pop(2,0) + state_next.base.p_hiv_pop(2,1)));
   }// end NS

}


template<typename ModelVariant, typename real_type>
void run_child_natural_history(int time_step,
                               const Parameters<ModelVariant, real_type> &pars,
                               const State<ModelVariant, real_type> &state_curr,
                               State<ModelVariant, real_type> &state_next,
                               IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          intermediate.children.hc_posthivmort(hd, cat, a, s) = state_next.children.hc1_hiv_pop(hd, cat, a, s) -
            (1.0 - cpars.ctx_effect * cpars.ctx_val(time_step)) * state_next.children.hc1_hiv_pop(hd, cat, a, s) * cpars.hc1_cd4_mort(hd, cat, a);
        }
      }
    }
  }

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = hc_ss.hc2_agestart; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc2DS; ++hd) {
          intermediate.children.hc_posthivmort(hd, cat, a, s) = state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) -
            (1.0 - cpars.ctx_effect * cpars.ctx_val(time_step)) * state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) * cpars.hc2_cd4_mort(hd, cat, a - hc_ss.hc2_agestart);
        }
      }
    }
  }

  //progress through CD4 categories
  for (int s = 0; s < ss.NS; ++s) {
    for (int hd = 1; hd < hc_ss.hc1DS; ++hd) {
      for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          intermediate.children.hc_grad(hd - 1, cat, a, s) -=  (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1) + state_next.children.hc1_hiv_pop(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1)) / 2; //moving to next cd4 category
          intermediate.children.hc_grad(hd, cat, a, s) += (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1) + state_next.children.hc1_hiv_pop(hd - 1, cat, a, s) * cpars.hc1_cd4_prog(hd - 1)) / 2; //moving into this cd4 category
        }
      }
    }
  }

  //progress through CD4 categories
  for (int s = 0; s < ss.NS; ++s) {
    for (int hd = 1; hd < hc_ss.hc2DS; ++hd) {
      for (int a = hc_ss.hc2_agestart; a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          intermediate.children.hc_grad(hd - 1, cat, a, s) -= (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc2_cd4_prog(hd - 1) + state_next.children.hc2_hiv_pop(hd - 1, cat, a - hc_ss.hc2_agestart, s) * cpars.hc2_cd4_prog(hd - 1)) / 2; //moving to next cd4 category
          intermediate.children.hc_grad(hd, cat, a, s) += (intermediate.children.hc_posthivmort(hd - 1, cat, a, s) * cpars.hc2_cd4_prog(hd - 1) + state_next.children.hc2_hiv_pop(hd - 1, cat, a - hc_ss.hc2_agestart, s) * cpars.hc2_cd4_prog(hd - 1)) / 2; //moving into this cd4 category
        }
      }
    }
  }
}

template<typename ModelVariant, typename real_type>
void run_child_hiv_mort(int time_step,
                        const Parameters<ModelVariant, real_type> &pars,
                        const State<ModelVariant, real_type> &state_curr,
                        State<ModelVariant, real_type> &state_next,
                        IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = 0; a < hc_ss.hc2_agestart; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          intermediate.children.hc_grad(hd, cat, a, s) -=
            state_next.children.hc1_hiv_pop(hd, cat, a, s) * cpars.hc1_cd4_mort(hd, cat, a);
        }
      }
    }
  }

  for (int s = 0; s < ss.NS; ++s) {
    for (int a = hc_ss.hc2_agestart; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
        for (int hd = 0; hd < hc_ss.hc2DS; ++hd) {
          intermediate.children.hc_grad(hd, cat, a, s) -=
            state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) *
            cpars.hc2_cd4_mort(hd, cat, a - hc_ss.hc2_agestart);
        }
      }
    }
  }
}

template<typename ModelVariant, typename real_type>
void add_child_grad(int time_step,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
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
void hc_initiate_art_by_age(int time_step,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  //all children under a certain age eligible for ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      if (a < cpars.hc_art_elig_age(time_step)) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
            if (a < hc_ss.hc2_agestart) {
              intermediate.children.hc_art_need(hd, cat, a, s) += state_next.children.hc1_hiv_pop(hd, cat, a, s);
            } else if (hd < hc_ss.hc2DS) {
              intermediate.children.hc_art_need(hd, cat, a, s) += state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s);
            }
          }
        } // end hc_ss.hc1DS
      } // end a
    } // end hcTT
  } // end ss.NS
}

template<typename ModelVariant, typename real_type>
void hc_initiate_art_by_cd4(int time_step,
                         const Parameters<ModelVariant, real_type> &pars,
                         const State<ModelVariant, real_type> &state_curr,
                         State<ModelVariant, real_type> &state_next,
                         IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  //all children under a certain CD4 eligible for ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
      for (int a = cpars.hc_art_elig_age(time_step); a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          if (hd >= (cpars.hc_art_elig_cd4(a, time_step))) {
            if (a < hc_ss.hc2_agestart) {
              intermediate.children.hc_art_need(hd, cat, a, s) += state_next.children.hc1_hiv_pop(hd, cat, a, s);
            } else if (hd < hc_ss.hc2DS) {
              intermediate.children.hc_art_need(hd, cat, a, s) += state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s);
            }
          }
        } // end hc_ss.hc1DS
      } // end a
    } // end hcTT
  } // end ss.NS
}

template<typename ModelVariant, typename real_type>
void hc_adjust_art_initiates_for_mort(int time_step,
                         const Parameters<ModelVariant, real_type> &pars,
                         const State<ModelVariant, real_type> &state_curr,
                         State<ModelVariant, real_type> &state_next,
                         IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          state_next.children.hc_art_num += intermediate.children.hc_art_need(hd, cat, a, s);
        } // end hcTT
      } // end hc_ss.hc1DS
    } // end a
  } // end ss.NS

  //how many should initialize ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          if (cpars.hc_art_val(time_step) > 0) {
            intermediate.children.hc_art_init(hd, cat, a, s) += intermediate.children.hc_art_need(hd, cat, a, s);
            for (int dur = 0; dur < ss.hTS; ++dur) {
              if (intermediate.children.hc_art_init(hd, cat, a, s) < 0) {
                intermediate.children.hc_art_init(hd, cat, a, s) = 0.0;
              }else{
                intermediate.children.hc_art_init(hd, cat, a, s) = intermediate.children.hc_art_init(hd, cat, a, s);
              }
            }// end ss.hTS
          }
        }// end hcTT
      }// end hc_ss.hc1DS
    }// end a
  }// end ss.NS

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          intermediate.children.hc_art_init_total += intermediate.children.hc_art_init(hd, cat, a, s);
        }// end hcTT
      }// end hc_ss.hc1DS
    }// end a
  }// end ss.NS

  //!!! TODO: fix order of for loop
  for (int s = 0; s <ss.NS; ++s) {
    for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        intermediate.children.hc_death_rate = 0.0;
        intermediate.children.hc_art_grad(0, hd, a, s) = 0.0;
        if (a < hc_ss.hc2_agestart) {
          if (state_next.children.hc1_art_pop(0, hd, a, s) >0) {
            intermediate.children.hc_death_rate =  cpars.hc_art_mort_rr(0, a, time_step) * 0.5 * (cpars.hc1_art_mort(hd, 0, a) + cpars.hc1_art_mort(hd, 1, a));
            state_next.children.hc1_art_aids_deaths(0,hd, a, s) =  intermediate.children.hc_death_rate * state_next.children.hc1_art_pop(0, hd, a, s);
            intermediate.children.hc_art_grad(0,hd, a, s) -= state_next.children.hc1_art_aids_deaths(0,hd, a, s);
            state_next.children.hc1_art_pop(0, hd,  a, s) += intermediate.children.hc_art_grad(0, hd, a, s);
          }
        } else if (hd < hc_ss.hc2DS && state_next.children.hc2_art_pop(0, hd, a-hc_ss.hc2_agestart, s) >0) {
          intermediate.children.hc_death_rate =  cpars.hc_art_mort_rr(0, a, time_step) * 0.5 * (cpars.hc2_art_mort(hd, 0, a-hc_ss.hc2_agestart) + cpars.hc2_art_mort(hd, 1, a-hc_ss.hc2_agestart));
          state_next.children.hc2_art_aids_deaths(0,hd, a-hc_ss.hc2_agestart, s) =  intermediate.children.hc_death_rate * state_next.children.hc2_art_pop(0, hd, a-hc_ss.hc2_agestart, s);
          intermediate.children.hc_art_grad(0, hd, a, s) -= state_next.children.hc2_art_aids_deaths(0,hd, a-hc_ss.hc2_agestart, s);
          state_next.children.hc2_art_pop(0, hd,  a-hc_ss.hc2_agestart, s) += intermediate.children.hc_art_grad(0, hd, a, s);
        }
      }// end a
    }// end hc_ss.hc1DS
  }// end ss.NS

  for (int s = 0; s <ss.NS; ++s) {
    for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        intermediate.children.hc_death_rate = 0;
        intermediate.children.hc_art_grad(2, hd, a, s) = 0.0;

        if (a < hc_ss.hc2_agestart) {
          intermediate.children.hc_death_rate = cpars.hc_art_mort_rr(2, a, time_step) * cpars.hc1_art_mort(hd, 2, a);
          state_next.children.hc1_art_aids_deaths(2,hd, a, s) =  state_next.children.hc1_art_pop(2, hd, a, s) * intermediate.children.hc_death_rate;
          intermediate.children.hc_art_grad(2,hd, a, s) -= state_next.children.hc1_art_aids_deaths(2,hd, a, s);
          state_next.children.hc1_art_pop(2, hd,  a, s) += intermediate.children.hc_art_grad(2, hd, a, s);
        } else if (hd < (hc_ss.hc2DS)) {
          intermediate.children.hc_death_rate = cpars.hc_art_mort_rr(2, a, time_step) * cpars.hc2_art_mort(hd, 2, a-hc_ss.hc2_agestart);
          state_next.children.hc2_art_aids_deaths(2,hd, a-hc_ss.hc2_agestart, s) =  state_next.children.hc2_art_pop(2, hd, a-hc_ss.hc2_agestart, s) * intermediate.children.hc_death_rate;
          intermediate.children.hc_art_grad(2,hd, a, s) -= state_next.children.hc2_art_aids_deaths(2,hd, a-hc_ss.hc2_agestart, s);
          state_next.children.hc2_art_pop(2, hd,  a-hc_ss.hc2_agestart, s) += intermediate.children.hc_art_grad(2, hd, a, s);
        }
      }// end a
    }// end hc_ss.hc1DS
  }// end ss.NS
}

template<typename ModelVariant, typename real_type>
void hc_art_num_num(int time_step,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  //Remove how many that are already on ART
  state_next.children.hc_art_num =  (cpars.hc_art_val(time_step) + cpars.hc_art_val(time_step-1)) / 2 ;
  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_num -= state_next.children.hc1_art_pop(dur, hd, a, s);
          }else if (hd < (hc_ss.hc2DS)) {
            state_next.children.hc_art_num -= state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);
          }
        }// end ss.hTS
      }// end hc_ss.hc1DS
    }// end a
  }// end ss.NS

  if (intermediate.children.hc_art_init_total < state_next.children.hc_art_num) {
    state_next.children.hc_art_num = intermediate.children.hc_art_init_total;
  }
  if (state_next.children.hc_art_num < 0) {
    state_next.children.hc_art_num =  0;
  }
}

template<typename ModelVariant, typename real_type>
void hc_art_pct_pct(int time_step,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_num += state_next.children.hc1_art_pop(dur, hd, a, s);
            state_next.children.hc_art_num += state_next.children.hc1_art_aids_deaths(dur,hd, a, s);
          }else if (hd < (hc_ss.hc2DS )) {
            state_next.children.hc_art_num += state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);
            state_next.children.hc_art_num += state_next.children.hc2_art_aids_deaths(dur,hd, a-hc_ss.hc2_agestart, s);
          }
        } //end ss.hTS
      } // end ss.hC1_disease_stages
    } // end a
  } // end ss.NS
  state_next.children.hc_art_num =  state_next.children.hc_art_num * (cpars.hc_art_val(time_step) + cpars.hc_art_val(time_step-1)) / 2;

  //Remove how many that are already on ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_num -= state_next.children.hc1_art_pop(dur, hd, a, s);
          }else if (hd < (hc_ss.hc2DS)) {
            state_next.children.hc_art_num -= state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);
          }
        } // end ss.hTS
      } // end ss.hC1_disease_stages
    } // end a
  } // end ss.NS
  if (intermediate.children.hc_art_init_total < state_next.children.hc_art_num) {
    state_next.children.hc_art_num = intermediate.children.hc_art_init_total;
  }
}

template<typename ModelVariant, typename real_type>
void hc_art_num_pct(int time_step,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  //Remove how many that are already on ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_num += state_next.children.hc1_art_pop(dur, hd, a, s) +  state_next.children.hc1_art_aids_deaths(dur,hd, a, s);
          } else if (hd < (hc_ss.hc2DS)) {
            state_next.children.hc_art_num += state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s) + state_next.children.hc2_art_aids_deaths(dur,hd, a-hc_ss.hc2_agestart, s);
          }
        } // end ss.hTS
      } // end hc_ss.hc1DS
    } //end a
  } //end ss.NS
  state_next.children.hc_art_num = (cpars.hc_art_val(time_step-1) + (state_next.children.hc_art_num * cpars.hc_art_val(time_step))) / 2;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_num -= state_next.children.hc1_art_pop(dur, hd, a, s);
          } else if (hd < (hc_ss.hc2DS )) {
            state_next.children.hc_art_num -= state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);
          }
        } // end ss.hTS
      } // end hc_ss.hc1DS
    } // end a
  } // end ss.NS

  if (state_next.children.hc_art_num < 0) {
    state_next.children.hc_art_num = 0;
  }
  if (intermediate.children.hc_art_init_total < state_next.children.hc_art_num) {
    state_next.children.hc_art_num = intermediate.children.hc_art_init_total;
  }
}

template<typename ModelVariant, typename real_type>
void hc_art_pct_num(int time_step,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_num -= state_next.children.hc1_art_pop(dur, hd, a, s);
          }else if (hd < (hc_ss.hc2DS)) {
            state_next.children.hc_art_num -= state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);
          }
        } // end ss.hTS
      } //end hc_ss.hc1DS
    } // end a
  } //end ss.NS

  state_next.children.hc_art_num = (state_curr.children.hc_art_num + cpars.hc_art_val(time_step)) / 2;

  //Remove how many that are already on ART
  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int dur = 0; dur < ss.hTS; ++dur) {
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc_art_num -= state_next.children.hc1_art_pop(dur, hd, a, s);
          } else if (hd < (hc_ss.hc2DS)) {
            state_next.children.hc_art_num -= state_next.children.hc2_art_pop(dur, hd, a-hc_ss.hc2_agestart, s);
          }
        } //end ss.hTS
      } //end hc_ss.hc1DS
    } //end a
  } //end ss.NS

  if (state_next.children.hc_art_num < 0) {
    state_next.children.hc_art_num = 0;
  }
  if (intermediate.children.hc_art_init_total < state_next.children.hc_art_num) {
    state_next.children.hc_art_num = intermediate.children.hc_art_init_total;
  }
}

template<typename ModelVariant, typename real_type>
void hc_art_initiation_by_age(int time_step,
                    const Parameters<ModelVariant, real_type> &pars,
                    const State<ModelVariant, real_type> &state_curr,
                    State<ModelVariant, real_type> &state_next,
                    IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int s = 0; s <ss.NS; ++s) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
        for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
          intermediate.children.hc_initByAge += intermediate.children.hc_art_init(hd, cat, a, s) * cpars.hc_art_init_dist(a, time_step);
        } //end hcTT
      } // end hc_ss.hc1DS
    } //end a
  } // end ss.NS
  if (intermediate.children.hc_initByAge == 0.0) {
    intermediate.children.hc_adj = 1.0 ;
  }else{
    intermediate.children.hc_adj = state_next.children.hc_art_num / intermediate.children.hc_initByAge;
  }
  for (int s = 0; s <ss.NS; ++s) {
    for (int cat = 0; cat < hc_ss.hcTT; ++cat) {
      for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
        for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
          if ((intermediate.children.hc_adj * cpars.hc_art_init_dist(a, time_step)) > 1.0) {
            intermediate.children.hc_art_scalar = 1.0;
          }else{
            intermediate.children.hc_art_scalar = intermediate.children.hc_adj * cpars.hc_art_init_dist(a, time_step);
          }
          if (state_next.children.hc_art_num > 0.0) {
            intermediate.children.hc_art_scalar = intermediate.children.hc_art_scalar;
          }else{
            intermediate.children.hc_art_scalar = 0.0;
          }
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc1_art_pop(0, hd, a, s) += intermediate.children.hc_art_scalar * intermediate.children.hc_art_init(hd, cat, a, s);
          }else if (hd < (hc_ss.hc2DS)) {
            state_next.children.hc2_art_pop(0, hd, a - hc_ss.hc2_agestart, s) += intermediate.children.hc_art_scalar * intermediate.children.hc_art_init(hd, cat, a, s);
          }
          if (a < hc_ss.hc2_agestart) {
            state_next.children.hc1_hiv_pop(hd, cat, a, s) -= intermediate.children.hc_art_scalar * intermediate.children.hc_art_init(hd, cat, a, s);
          }else if (hd < (hc_ss.hc2DS )) {
            state_next.children.hc2_hiv_pop(hd, cat, a - hc_ss.hc2_agestart, s) -=  intermediate.children.hc_art_scalar * intermediate.children.hc_art_init(hd, cat, a, s);
          }

        } //end hc_ss.hc1DS
      } // end a
    } // end hcTT
  } // end ss.NS
}

template<typename ModelVariant, typename real_type>
void run_child_art_initiation(int time_step,
                              const Parameters<ModelVariant, real_type> &pars,
                              const State<ModelVariant, real_type> &state_curr,
                              State<ModelVariant, real_type> &state_next,
                              IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  internal::hc_initiate_art_by_age(time_step, pars, state_curr, state_next, intermediate);
  internal::hc_initiate_art_by_cd4(time_step, pars, state_curr, state_next, intermediate);
  internal::hc_adjust_art_initiates_for_mort(time_step, pars, state_curr, state_next, intermediate);

  //Progress ART to the correct time on ART
  for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int s = 0; s <ss.NS; ++s) {
        if (a < hc_ss.hc2_agestart) {
          if (state_next.children.hc1_art_pop(0, hd, a, s) > 0) {
            state_next.children.hc1_art_pop(1, hd, a, s) += state_next.children.hc1_art_pop(0, hd, a, s);
            state_next.children.hc1_art_pop(0, hd, a, s) -= state_next.children.hc1_art_pop(0, hd, a, s);
          }
        }else if (hd < (hc_ss.hc2DS)) {
            state_next.children.hc2_art_pop(1, hd, a-hc_ss.hc2_agestart, s) += state_next.children.hc2_art_pop(0, hd, a-hc_ss.hc2_agestart, s);
            state_next.children.hc2_art_pop(0, hd, a-hc_ss.hc2_agestart, s) -= state_next.children.hc2_art_pop(0, hd, a-hc_ss.hc2_agestart, s);
          }
      }//end ss.NS
    }// end a
  }// end hc_ss.hc1DS

  if (!cpars.hc_art_isperc(time_step) && !cpars.hc_art_isperc(time_step-1)) { // both numbers
   internal::hc_art_num_num(time_step, pars, state_curr, state_next, intermediate);
  } else if (cpars.hc_art_isperc(time_step) && cpars.hc_art_isperc(time_step-1)) { // both percentages
    internal::hc_art_pct_pct(time_step, pars, state_curr, state_next, intermediate);
  } else if (cpars.hc_art_isperc(time_step) && !cpars.hc_art_isperc(time_step-1)) { // num to percentage
    internal::hc_art_num_pct(time_step, pars, state_curr, state_next, intermediate);
  } else if (cpars.hc_art_isperc(time_step-1) && !cpars.hc_art_isperc(time_step)) { //percentage to num
    internal::hc_art_pct_num(time_step, pars, state_curr, state_next, intermediate);
  }

  internal::hc_art_initiation_by_age(time_step, pars, state_curr, state_next, intermediate);
}


template<typename ModelVariant, typename real_type>
void run_child_art_mortality(int time_step,
                             const Parameters<ModelVariant, real_type> &pars,
                             const State<ModelVariant, real_type> &state_curr,
                             State<ModelVariant, real_type> &state_next,
                             IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_hiv_child_infections can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  constexpr auto hc_ss = StateSpace<ModelVariant>().children;
  const auto cpars = pars.children.children;

  for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int s = 0; s <ss.NS; ++s) {
        intermediate.children.hc_death_rate = 0.0;
        intermediate.children.hc_art_grad(0, hd, a, s) = 0.0;
        if (a < hc_ss.hc2_agestart) {
          if (state_next.children.hc1_art_pop(0, hd, a, s) > 0) {
            intermediate.children.hc_death_rate = cpars.hc_art_mort_rr(0, a, time_step) * 0.5 * (cpars.hc1_art_mort(hd, 0, a) + cpars.hc1_art_mort(hd, 1, a));
            state_next.children.hc1_art_aids_deaths(0, hd, a, s) = intermediate.children.hc_death_rate * state_next.children.hc1_art_pop(0, hd, a, s);
            intermediate.children.hc_art_grad(0, hd, a, s) -= state_next.children.hc1_art_aids_deaths(0, hd, a, s);
            state_next.children.hc1_art_pop(0, hd, a, s) += intermediate.children.hc_art_grad(0, hd, a, s);
          }
        } else if (hd < hc_ss.hc2DS) {
          intermediate.children.hc_death_rate = cpars.hc_art_mort_rr(0, a, time_step) * 0.5 * (cpars.hc2_art_mort(hd, 0, a-hc_ss.hc2_agestart) + cpars.hc2_art_mort(hd, 1, a-hc_ss.hc2_agestart));
          state_next.children.hc2_art_aids_deaths(0, hd, a-hc_ss.hc2_agestart, s)  = intermediate.children.hc_death_rate * state_next.children.hc2_art_pop(0, hd, a-hc_ss.hc2_agestart, s);
          intermediate.children.hc_art_grad(0, hd, a, s) -= state_next.children.hc2_art_aids_deaths(0, hd, a-hc_ss.hc2_agestart, s);
          state_next.children.hc2_art_pop(0, hd, a-hc_ss.hc2_agestart, s) += intermediate.children.hc_art_grad(0, hd, a, s);
        }
      }//end ss.NS
    }// end a
  }// end hc_ss.hc1DS

  for (int hd = 0; hd < hc_ss.hc1DS; ++hd) {
    for (int a = 0; a < pars.base.options.p_idx_fertility_first; ++a) {
      for (int s = 0; s <ss.NS; ++s) {
        if (a < hc_ss.hc2_agestart) {
          if (state_next.children.hc1_art_pop(1, hd, a, s) > 0) {
            state_next.children.hc1_art_pop(2, hd, a, s) += state_next.children.hc1_art_pop(1, hd, a, s);
            state_next.children.hc1_art_pop(1, hd, a, s) -= state_next.children.hc1_art_pop(1, hd, a, s);
          }
        } else if (hd < (hc_ss.hc2DS) && state_next.children.hc2_art_pop(1, hd, a-hc_ss.hc2_agestart, s) > 0) {
          state_next.children.hc2_art_pop(2, hd, a-hc_ss.hc2_agestart, s) += state_next.children.hc2_art_pop(1, hd, a-hc_ss.hc2_agestart, s);
          state_next.children.hc2_art_pop(1, hd, a-hc_ss.hc2_agestart, s) -= state_next.children.hc2_art_pop(1, hd, a-hc_ss.hc2_agestart, s);
        }
      }//end ss.NS
    }// end a
  }// end hc_ss.hc1DS
}


template<typename ModelVariant, typename real_type>
void run_wlhiv_births(int time_step,
                      const Parameters<ModelVariant, real_type> &pars,
                      const State<ModelVariant, real_type> &state_curr,
                      State<ModelVariant, real_type> &state_next,
                      IntermediateData<ModelVariant, real_type> &intermediate) {
  static_assert(ModelVariant::run_child_model,
                "run_wlhiv_births can only be called for model variants where run_child_model is true");
  constexpr auto ss = StateSpace<ModelVariant>().base;
  const auto cpars = pars.children.children;
  const auto demog = pars.base.demography;

  intermediate.children.asfr_sum = 0.0;
  for (int a = 0; a < pars.base.options.p_fertility_age_groups; ++a) {
    intermediate.children.asfr_sum += demog.age_specific_fertility_rate(a, time_step);
  } // end a


  intermediate.children.births_sum = state_next.base.births;


  for (int a = 0; a < pars.base.options.p_fertility_age_groups; ++a) {
      intermediate.children.nHIVcurr = 0.0;
      intermediate.children.nHIVlast = 0.0;
      intermediate.children.df = 0.0;

      for (int hd = 0; hd < ss.hDS; ++hd) {
        intermediate.children.nHIVcurr += state_next.base.h_hiv_adult(hd, a, 1);
        intermediate.children.nHIVlast += state_curr.base.h_hiv_adult(hd, a, 1);
        for (int ht = 0; ht < ss.hTS; ++ht) {
          intermediate.children.nHIVcurr += state_next.base.h_art_adult(ht, hd, a, 1);
          intermediate.children.nHIVlast += state_curr.base.h_art_adult(ht, hd, a, 1);
        } //end hTS
      } //end hDS

      intermediate.children.prev = intermediate.children.nHIVcurr / state_next.base.p_total_pop(a + 15,1);

      for (int hd = 0; hd < ss.hDS; ++hd) {
        intermediate.children.df += cpars.local_adj_factor * cpars.fert_mult_by_age(a) * cpars.fert_mult_offart(hd) * ((state_next.base.h_hiv_adult(hd, a, 1) + state_curr.base.h_hiv_adult(hd, a, 1)) / 2);
        //women on ART less than 6 months use the off art fertility multiplier
        intermediate.children.df += cpars.local_adj_factor * cpars.fert_mult_by_age(a) * cpars.fert_mult_offart(hd) * ((state_next.base.h_art_adult(0, hd, a, 1) + state_curr.base.h_art_adult(0, hd, a, 1)) / 2);
        for (int ht = 1; ht < ss.hTS; ++ht) {
          intermediate.children.df += cpars.local_adj_factor * cpars.fert_mult_onart(a) * ((state_next.base.h_art_adult(ht, hd, a, 1) + state_curr.base.h_art_adult(ht, hd, a, 1)) / 2);
        } //end hTS
      } // end hDS


      if (intermediate.children.nHIVcurr > 0) {
        intermediate.children.df = intermediate.children.df / ((intermediate.children.nHIVcurr + intermediate.children.nHIVlast) / 2);
      }else{
        intermediate.children.df = 1;
      }

    for (int hd = 0; hd < ss.hDS; ++hd) {
      intermediate.children.df += cpars.local_adj_factor * cpars.fert_mult_by_age(a) * cpars.fert_mult_offart(hd) * ((state_next.base.h_hiv_adult(hd, a, 1) + state_curr.base.h_hiv_adult(hd, a, 1)) / 2);
      //women on ART less than 6 months use the off art fertility multiplier
       intermediate.children.df += cpars.local_adj_factor * cpars.fert_mult_by_age(a) * cpars.fert_mult_offart(hd) * ((state_next.base.h_art_adult(0, hd, a, 1) + state_curr.base.h_art_adult(0, hd, a, 1)) / 2);
       for (int ht = 1; ht < ss.hTS; ++ht) {
         intermediate.children.df += cpars.local_adj_factor * cpars.fert_mult_onart(a) * ((state_next.base.h_art_adult(ht, hd, a, 1) + state_curr.base.h_art_adult(ht, hd, a, 1)) / 2);
      } //end hTS
    } // end hDS


      intermediate.children.birthsCurrAge = (intermediate.children.nHIVcurr + intermediate.children.nHIVlast) / 2 * cpars.total_fertility_rate(time_step) * intermediate.children.df / (intermediate.children.df * intermediate.children.prev + 1 - intermediate.children.prev) *  demog.age_specific_fertility_rate(a, time_step) / intermediate.children.asfr_sum ;
      intermediate.children.birthsHE += intermediate.children.birthsCurrAge;
      if (a < 9) {
        intermediate.children.births_HE_15_24 += intermediate.children.birthsCurrAge;
      }

    } // end a


//MKW at this point this is giving NAN at 1986
  state_next.children.hiv_births = intermediate.children.birthsHE;

}


}// namespace internal

template<typename ModelVariant, typename real_type>
void run_child_model_simulation(int time_step,
                                const Parameters<ModelVariant, real_type> &pars,
                                const State<ModelVariant, real_type> &state_curr,
                                State<ModelVariant, real_type> &state_next,
                                internal::IntermediateData<ModelVariant, real_type> &intermediate) {

  // internal::run_child_ageing(time_step, pars, state_curr, state_next, intermediate);
  // internal::run_child_natural_history(time_step, pars, state_curr, state_next, intermediate);
  // internal::run_child_hiv_mort(time_step, pars, state_curr, state_next, intermediate);
  // internal::add_child_grad(time_step, pars, state_curr, state_next, intermediate);
  // //This is only the order for nosocomial infections
  // //order from mkw_bf_trans: run_child_ageing, run_wlhiv_births, run_child_hiv_infections, run_child_natural_history,
  // //run_child_hiv_mort, add_child_trad, run_child_art_initiation, run_child_art_mortality
  // internal::run_child_hiv_infections(time_step, pars, state_curr, state_next, intermediate);
  // // !!!TODO: also need to fix the looping order for some loops
  // // !!!TODO: put this in an if statement to only run if the first year of ART has passed
  // internal::run_child_art_initiation(time_step, pars, state_curr, state_next, intermediate);
  // internal::run_child_art_mortality(time_step, pars, state_curr, state_next, intermediate);
  // internal::run_wlhiv_births(time_step, pars, state_curr, state_next, intermediate);

  internal::run_child_ageing(time_step, pars, state_curr, state_next, intermediate);
  internal::run_wlhiv_births(time_step, pars, state_curr, state_next, intermediate);
  internal::run_child_hiv_infections(time_step, pars, state_curr, state_next, intermediate);
  internal::run_child_natural_history(time_step, pars, state_curr, state_next, intermediate);
  internal::run_child_hiv_mort(time_step, pars, state_curr, state_next, intermediate);
  internal::add_child_grad(time_step, pars, state_curr, state_next, intermediate);
  //This is only the order for nosocomial infections
  //order from mkw_bf_trans: run_child_ageing, run_wlhiv_births, run_child_hiv_infections, run_child_natural_history,
  //run_child_hiv_mort, add_child_trad, run_child_art_initiation, run_child_art_mortality
  // !!!TODO: also need to fix the looping order for some loops
  // !!!TODO: put this in an if statement to only run if the first year of ART has passed
  internal::run_child_art_initiation(time_step, pars, state_curr, state_next, intermediate);
  internal::run_child_art_mortality(time_step, pars, state_curr, state_next, intermediate);
  }


} // namespace leapfrog
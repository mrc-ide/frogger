#pragma once

#include "intermediate_data.hpp"
#include "state_space.hpp"
#include "model_variants.hpp"
#include "generated/parameter_types.hpp"

namespace leapfrog {

template<typename real_type>
struct Options {
  int hts_per_year;
  double dt;
  const int p_idx_fertility_first;
  const int p_fertility_age_groups;
  const int p_idx_hiv_first_adult;
  const int adult_incidence_first_age_group;
  const int pAG_INCIDPOP;
  const int ts_art_start;
  const int hAG_15plus;
  const int hIDX_15PLUS;
  const int proj_period_int;

  Options(int hts_per_year,
          int ts_art_start,
          int hAG_15plus,
	  int proj_period_int) :
      hts_per_year(hts_per_year),
      dt(1.0 / hts_per_year),
      p_idx_fertility_first(15),
      p_fertility_age_groups(35),
      p_idx_hiv_first_adult(15),
      adult_incidence_first_age_group(15),
      pAG_INCIDPOP(35),
      ts_art_start(ts_art_start),
      hAG_15plus(hAG_15plus),
      hIDX_15PLUS(0),
      proj_period_int(proj_period_int) {}
};

template<typename real_type>
struct BaseModelParameters {
  Options<real_type> options;
  Demography<real_type> demography;
  Incidence<real_type> incidence;
  NaturalHistory<real_type> natural_history;
  Art<real_type> art;
};

template<typename ModelVariant, typename real_type>
struct ChildModelParameters {
};

template<typename real_type>
struct ChildModelParameters<ChildModel, real_type> {
  Children<real_type> children;
};

template<typename ModelVariant, typename real_type>
struct Parameters {
  BaseModelParameters<real_type> base;
  ChildModelParameters<ModelVariant, real_type> children;
};

}

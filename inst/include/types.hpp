#pragma once

#include <unsupported/Eigen/CXX11/Tensor>
#include "state_space.hpp"

namespace leapfrog {

template<typename real_type>
using TensorMap1 = Eigen::TensorMap <Eigen::Tensor<real_type, 1>>;

template<typename real_type>
using TensorMap2 = Eigen::TensorMap <Eigen::Tensor<real_type, 2>>;

template<typename real_type>
using TensorMap3 = Eigen::TensorMap <Eigen::Tensor<real_type, 3>>;

template<typename real_type>
using TensorMap4 = Eigen::TensorMap <Eigen::Tensor<real_type, 4>>;

template<typename real_type>
using Tensor1 = Eigen::Tensor<real_type, 1>;

template<typename real_type>
using Tensor2 = Eigen::Tensor<real_type, 2>;

template<typename real_type>
using Tensor3 = Eigen::Tensor<real_type, 3>;

template<typename real_type>
using Tensor4 = Eigen::Tensor<real_type, 4>;

template<typename real_type>
using Tensor5 = Eigen::Tensor<real_type, 5>;

template<typename real_type>
struct Parameters {
  // First index of population eligible for fertility
  int fertility_first_age_group;
  // Number of ages eligible for fertility
  int age_groups_fert;
  // Numer of age groups in HIV 15+ population
  int age_groups_hiv_15plus;
  // First index of HIV population to model as adult
  int hiv_adult_first_age_group;
  // Time step to start ART treatment
  int time_art_start;
  // Index of the youngest age that is reflected in the adult incidence input
  int adult_incidence_first_age_group;

  int pAG_INCIDPOP;
  // Number of time steps per year in the HIV projection
  int hiv_steps_per_year;
  // Difference in time for each hiv time step in HIV projection
  double dt;

  int scale_cd4_mortality;

  int hIDX_15PLUS;

  real_type art_alloc_mxweight;

  // Incidence rate at each time step
  TensorMap1<real_type> incidence_rate;

  TensorMap2<real_type> base_pop;
  TensorMap3<real_type> survival;
  TensorMap3<real_type> net_migration;
  TensorMap2<real_type> age_sex_fertility_ratio;
  TensorMap2<real_type> births_sex_prop;
  TensorMap3<real_type> incidence_relative_risk_age;
  TensorMap1<real_type> incidence_relative_risk_sex;
  TensorMap3<real_type> cd4_mortality;
  TensorMap3<real_type> cd4_progression;
  Tensor1<int> artcd4elig_idx;
  TensorMap3<real_type> cd4_initdist;
  TensorMap4<real_type> art_mortality;
  TensorMap2<real_type> artmx_timerr;
  Tensor1<real_type> h_art_stage_dur;
  TensorMap1<real_type> art_dropout;
  TensorMap2<real_type> art15plus_num;
  TensorMap2<int> art15plus_isperc;
};

template<HivAgeStratification S>
constexpr size_t num_genders = StateSpace<S>::num_genders;

template<HivAgeStratification S>
constexpr size_t age_groups_pop = StateSpace<S>::age_groups_pop;

template<HivAgeStratification S>
constexpr size_t age_groups_hiv = StateSpace<S>::age_groups_hiv;

template<HivAgeStratification S>
constexpr size_t disease_stages = StateSpace<S>::disease_stages;

template<HivAgeStratification S>
constexpr size_t treatment_stages = StateSpace<S>::treatment_stages;

namespace {
using Eigen::Sizes;
using Eigen::TensorFixedSize;
}

template<HivAgeStratification S, typename real_type>
struct State {
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> total_population;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> natural_deaths;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> hiv_population;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> hiv_natural_deaths;
  TensorFixedSize <real_type, Sizes<disease_stages<S>, age_groups_hiv<S>, num_genders<S>>> hiv_strat_adult;
  TensorFixedSize <real_type, Sizes<treatment_stages<S>, disease_stages<S>, age_groups_hiv<S>, num_genders<S>>>
      art_strat_adult;
  real_type births;
  TensorFixedSize <real_type, Sizes<disease_stages<S>, age_groups_hiv<S>, num_genders<S>>> aids_deaths_no_art;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> infections;
  TensorFixedSize <real_type, Sizes<treatment_stages<S>, disease_stages<S>, age_groups_hiv<S>, num_genders<S>>>
      aids_deaths_art;
  TensorFixedSize <real_type, Sizes<disease_stages<S>, age_groups_hiv<S>, num_genders<S>>> art_initiation;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> hiv_deaths;

  State() {}

  void reset() {
    total_population.setZero();
    natural_deaths.setZero();
    hiv_population.setZero();
    hiv_natural_deaths.setZero();
    hiv_strat_adult.setZero();
    art_strat_adult.setZero();
    aids_deaths_no_art.setZero();
    infections.setZero();
    aids_deaths_art.setZero();
    art_initiation.setZero();
    hiv_deaths.setZero();
    births = 0;
  }
};

namespace internal {

const int MALE = 0;
const int FEMALE = 1;
const int ART0MOS = 0;

template<HivAgeStratification S, typename real_type>
struct IntermediateData {
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> migration_rate;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> hiv_net_migration;
  TensorFixedSize <real_type, Sizes<age_groups_hiv<S>, num_genders<S>>> hiv_population_coarse_ages;
  TensorFixedSize <real_type, Sizes<age_groups_hiv<S>, num_genders<S>>> hiv_age_up_prob;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> hiv_negative_pop;
  TensorFixedSize <real_type, Sizes<age_groups_pop<S>, num_genders<S>>> infections_ts;
  TensorFixedSize <real_type, Sizes<num_genders<S>>> incidence_rate_sex;
  TensorFixedSize <real_type, Sizes<num_genders<S>>> hiv_neg_aggregate;
  TensorFixedSize <real_type, Sizes<num_genders<S>>> Xhivn_incagerr;
  TensorFixedSize <real_type, Sizes<age_groups_hiv<S>, num_genders<S>>> hiv_deaths_age_sex;
  TensorFixedSize <real_type, Sizes<disease_stages<S>, age_groups_hiv<S>, num_genders<S>>> grad;
  TensorFixedSize <real_type, Sizes<treatment_stages<S>, disease_stages<S>, age_groups_hiv<S>, num_genders<S>>> gradART;
  Tensor2<real_type> artelig_hahm;
  TensorFixedSize <real_type, Sizes<age_groups_hiv<S>>> hivpop_ha;
  real_type cd4mx_scale;
  real_type artpop_hahm;
  real_type deaths;
  int everARTelig_idx;
  int cd4elig_idx;
  real_type infections_a;
  real_type infections_ha;
  real_type deaths_art;
  real_type Xart_15plus;
  real_type Xartelig_15plus;
  real_type expect_mort_artelig15plus;
  int anyelig_idx;
  real_type artnum_hts;
  real_type artcov_hts;
  real_type curr_coverage;
  real_type artinit_hts;
  real_type artinit_hahm;
  real_type hivqx_ha;
  real_type hivdeaths_a;

  IntermediateData(int age_groups_hiv_15plus)
      :
      artelig_hahm(disease_stages<S>, age_groups_hiv_15plus),
      cd4mx_scale(1.0),
      artpop_hahm(0.0),
      deaths(0.0),
      everARTelig_idx(0),
      cd4elig_idx(0),
      infections_a(0.0),
      infections_ha(0.0),
      deaths_art(0.0),
      Xart_15plus(0.0),
      Xartelig_15plus(0.0),
      expect_mort_artelig15plus(0.0),
      anyelig_idx(0),
      artnum_hts(0.0),
      artcov_hts(0.0),
      curr_coverage(0.0),
      artinit_hts(0.0),
      artinit_hahm(0.0),
      hivqx_ha(0.0),
      hivdeaths_a(0.0) {}

  void reset() {
    migration_rate.setZero();
    hiv_net_migration.setZero();
    hiv_population_coarse_ages.setZero();
    hiv_age_up_prob.setZero();
    hiv_negative_pop.setZero();
    infections_ts.setZero();
    hiv_neg_aggregate.setZero();
    Xhivn_incagerr.setZero();
    incidence_rate_sex.setZero();
    hiv_deaths_age_sex.setZero();
    grad.setZero();
    gradART.setZero();
    artelig_hahm.setZero();
    hivpop_ha.setZero();
    cd4mx_scale = 1.0;
    deaths = 0.0;
    everARTelig_idx = 0;
    cd4elig_idx = 0;
    infections_a = 0.0;
    infections_ha = 0.0;
    deaths_art = 0.0;
    Xart_15plus = 0.0;
    Xartelig_15plus = 0.0;
    expect_mort_artelig15plus = 0.0;
    anyelig_idx = 0;
    artnum_hts = 0.0;
    artcov_hts = 0.0;
    curr_coverage = 0.0;
    artinit_hts = 0.0;
    artinit_hahm = 0.0;
    hivqx_ha = 0.0;
    hivdeaths_a = 0.0;
  }
};

}
}

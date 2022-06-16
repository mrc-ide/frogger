#pragma once

#include <unsupported/Eigen/CXX11/Tensor>

template <typename real_type>
using TensorMap1 = Eigen::TensorMap<Eigen::Tensor<real_type, 1>>;
template <typename real_type>
using TensorMap2 = Eigen::TensorMap<Eigen::Tensor<real_type, 2>>;

template <typename real_type>
using TensorMap3 = Eigen::TensorMap<Eigen::Tensor<real_type, 3>>;

template <typename real_type>
using Tensor2 = Eigen::Tensor<real_type, 2>;

template <typename real_type>
using Tensor3 = Eigen::Tensor<real_type, 3>;

template <typename real_type>
using Tensor4 = Eigen::Tensor<real_type, 4>;

template <typename real_type>
struct Parameters {
  int num_genders;
  int age_groups_pop;             // Default 81 for ages 0 to 80+
  int fertility_first_age_group;  // First index of population eligible for
                                  // fertility
  int age_groups_fert;            // Number of ages eligible for fertility
  int age_groups_hiv;             // Numer of age groups in HIV population
  int disease_stages;             // Number of HIV disease stages
  int hiv_adult_first_age_group;  // First index of HIV population to model as
                                  // adult
  int treatment_stages;           // Number of HIV ART treatment stages
  int time_art_start;             // Time step to start ART treatment
  TensorMap1<int> age_groups_hiv_span;  // Number of years in each HIV age group

  TensorMap2<real_type> base_pop;
  TensorMap3<real_type> survival;
  TensorMap3<real_type> net_migration;
  TensorMap2<real_type> age_sex_fertility_ratio;
  TensorMap2<real_type> births_sex_prop;
};

template <typename real_type>
struct State {
  Tensor2<real_type> total_population;
  Tensor2<real_type> natural_deaths;
  Tensor2<real_type> hiv_population;
  Tensor2<real_type> hiv_natural_deaths;
  Tensor3<real_type> hiv_strat_adult;
  Tensor4<real_type> art_strat_adult;
  real_type births;

  State(int age_groups_pop,
        int num_genders,
        int disease_stages,
        int age_groups_hiv,
        int treatment_stages)
      : total_population(age_groups_pop, num_genders),
        natural_deaths(age_groups_pop, num_genders),
        hiv_population(age_groups_pop, num_genders),
        hiv_natural_deaths(age_groups_pop, num_genders),
        hiv_strat_adult(disease_stages, age_groups_hiv, num_genders),
        art_strat_adult(treatment_stages,
                        disease_stages,
                        age_groups_hiv,
                        num_genders) {}
};

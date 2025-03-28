#pragma once

namespace leapfrog {

template<typename ModelVariant>
concept MV = requires (ModelVariant mv) {
  { mv.run_demographic_projection } -> std::convertible_to<bool>;
  { mv.run_hiv_simulation } -> std::convertible_to<bool>;
  { mv.use_coarse_stratification } -> std::convertible_to<bool>;
  { mv.run_child_model } -> std::convertible_to<bool>;
};


template<typename Config>
concept RunDemographicProjection = MV<typename Config::ModelVariant> && Config::ModelVariant::run_demographic_projection;

template<typename Config>
concept RunHivSimulation = MV<typename Config::ModelVariant> && Config::ModelVariant::run_hiv_simulation;

template<typename Config>
concept UseCoarseStratification = MV<typename Config::ModelVariant> && Config::ModelVariant::use_coarse_stratification;

template<typename Config>
concept RunChildModel = MV<typename Config::ModelVariant> && Config::ModelVariant::run_child_model;

}

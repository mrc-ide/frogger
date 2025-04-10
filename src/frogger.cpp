#include <Rcpp.h>

#include "frogger.hpp"
#include "generated/r_interface/r_adapters.hpp"

template<typename real_type, typename ModelVariant>
Rcpp::List simulate_model(
  const Rcpp::List& data,
  const int time_steps,
  const int hiv_steps,
  const std::vector<int> save_steps,
  const bool is_midyear_projection,
  const int t_ART_start
) {
  using LF = leapfrog::Leapfrog<leapfrog::R, real_type, ModelVariant>;

  const auto opts = leapfrog::get_opts<real_type>(hiv_steps, t_ART_start, is_midyear_projection);
  const auto pars = LF::Cfg::get_pars(data, opts, time_steps);

  auto state = LF::run_model(time_steps, save_steps, pars, opts);

  const int output_size = LF::Cfg::get_build_output_size(0);
  Rcpp::List ret(output_size);
  Rcpp::CharacterVector names(output_size);
  LF::Cfg::build_output(0, state, ret, names, save_steps.size());
  ret.attr("names") = names;

  return ret;
}

// [[Rcpp::export]]
Rcpp::List run_base_model(
  const Rcpp::List data,
  const std::string model_variant,
  const int time_steps,
  const int hiv_steps,
  const std::vector<int> save_steps,
  const bool is_midyear_projection,
  const int t_ART_start
) {
  // TODO Mantra write docs, save_steps now must be 0 index based, nothing is nullable, projection_period is "calendar" or "midyear", t_ART_start is 0 based (so substract 1 from R value)
  if (model_variant == "DemographicProjection") {
    return simulate_model<double, leapfrog::DemographicProjection>(data, time_steps, hiv_steps, save_steps, is_midyear_projection, t_ART_start);
  } else if (model_variant == "HivFullAgeStratification") {
    return simulate_model<double, leapfrog::HivFullAgeStratification>(data, time_steps, hiv_steps, save_steps, is_midyear_projection, t_ART_start);
  } else if (model_variant == "HivCoarseAgeStratification") {
    return simulate_model<double, leapfrog::HivCoarseAgeStratification>(data, time_steps, hiv_steps, save_steps, is_midyear_projection, t_ART_start);
  } else if (model_variant == "ChildModel") {
    return simulate_model<double, leapfrog::ChildModel>(data, time_steps, hiv_steps, save_steps, is_midyear_projection, t_ART_start);
  } else {
    throw std::runtime_error(
      "Invalid model_variant: " + model_variant +
      ". It must be one of" +
      "'DemographicProjection', " +
      "'HivFullAgeStratification', " +
      "'HivCoarseAgeStratification', " +
      "'ChildModel'"
    );
  }
}

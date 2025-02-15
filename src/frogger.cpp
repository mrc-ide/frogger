#include <Rcpp.h>

#include "frogger.hpp"
#include "intermediate_data.hpp"
#include "model_variants.hpp"
#include "state_space.hpp"
#include "generated/model_input.hpp"
#include "generated/model_output.hpp"
#include "r_utils.hpp"

int transform_simulation_years(
    const Rcpp::List demp,
    Rcpp::Nullable<Rcpp::NumericVector> r_sim_years) {

  Rcpp::NumericVector Sx = demp["Sx"];
  Rcpp::Dimension d = Sx.attr("dim");
  // Simulation initialises state from first years input data (index 0)
  // then runs for each year simulating this years (i) data using previous years
  // state (i - 1) and this years input data (i).
  const int max_sim_years = d[2];
  if (r_sim_years.isNull()) {
    return max_sim_years;
  }
  Rcpp::NumericVector years(r_sim_years);
  auto sim_years = years.length();
  if (sim_years > max_sim_years) {
    Rcpp::stop("No of years > max years of " + std::to_string(max_sim_years));
  }
  return sim_years;
}

int transform_hts_per_year(SEXP r_hts_per_year) {
  int hts_per_year;
  if (r_hts_per_year == R_NilValue) {
    hts_per_year = 10;
  } else {
    hts_per_year = INTEGER(r_hts_per_year)[0];
  }
  return hts_per_year;
}

std::vector<int> transform_output_steps(Rcpp::NumericVector output_steps) {
  auto out = Rcpp::as<std::vector<int>>(output_steps);
  convert_0_based(out);
  return out;
}

template<typename ModelVariant>
Rcpp::List simulate_model(const leapfrog::StateSpace<ModelVariant> ss,
                          const Rcpp::List data,
                          const int proj_years,
                          const int hiv_steps,
                          const std::vector<int> save_steps) {

  int projection_period_int = leapfrog::internal::PROJPERIOD_CALENDAR;
  Rcpp::String projection_period_str = data["projection_period"];
  if (projection_period_str == "calendar") {
    projection_period_int = leapfrog::internal::PROJPERIOD_CALENDAR;
  } else if (projection_period_str == "midyear") {
    projection_period_int = leapfrog::internal::PROJPERIOD_MIDYEAR;
  } else {
    Rf_error("projection_period \"%s\" not found. Please select \"midyear\" or \"calendar\".\n",
	     projection_period_str.get_cstring());
  }

  const leapfrog::Options<double> opts = {
      hiv_steps,
      Rcpp::as<int>(data["t_ART_start"]) - 1,
      ss.hiv.hAG,
      projection_period_int
  };

  const auto params = setup_model_params<ModelVariant, double>(data, opts, proj_years);

  auto state = leapfrog::run_model<ModelVariant, double>(proj_years, save_steps, params);

  auto ret = build_r_output<ModelVariant, double>(state, save_steps);

  return ret;
}

// [[Rcpp::export]]
Rcpp::List run_base_model(const Rcpp::List data,
                          Rcpp::Nullable<Rcpp::NumericVector> sim_years,
                          SEXP hts_per_year,
                          Rcpp::NumericVector output_steps,
                          std::string model_variant) {
  const int proj_years = transform_simulation_years(data, sim_years);
  const std::vector<int> save_steps = transform_output_steps(output_steps);
  const int hiv_steps = transform_hts_per_year(hts_per_year);

  Rcpp::List ret;
  if (model_variant == "DemographicProjection") {
    constexpr auto ss = leapfrog::StateSpace<leapfrog::DemographicProjection>();
    ret = simulate_model<leapfrog::DemographicProjection>(ss, data, proj_years, hiv_steps, save_steps);
  } else if (model_variant == "ChildModel") {
    constexpr auto ss = leapfrog::StateSpace<leapfrog::ChildModel>();
    ret = simulate_model<leapfrog::ChildModel>(ss, data, proj_years, hiv_steps, save_steps);
  } else if (model_variant == "HivFullAgeStratification") {
    constexpr auto ss = leapfrog::StateSpace<leapfrog::HivFullAgeStratification>();
    ret = simulate_model<leapfrog::HivFullAgeStratification>(ss, data, proj_years, hiv_steps, save_steps);
  } else if (model_variant == "HivCoarseAgeStratification") {
    constexpr auto ss = leapfrog::StateSpace<leapfrog::HivCoarseAgeStratification>();
    ret = simulate_model<leapfrog::HivCoarseAgeStratification>(ss, data, proj_years, hiv_steps, save_steps);
  } else {
    Rcpp::stop("Invalid model variant " + model_variant + " must be one of " +
               "'DemographicProjection', 'HivFullAgeStratification', 'HivCoarseAgeStratification' or 'ChildModel'");
  }

  return ret;
}

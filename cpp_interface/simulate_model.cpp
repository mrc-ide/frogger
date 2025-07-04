#include "H5Cpp.h"
#include "array/array.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string_view>
#include <numeric>

#include "frogger.hpp"
#include "generated/cpp_interface/cpp_adapters.hpp"

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout <<
              "Usage: simulate_model <sim_years> <params_file> <output_dir>"
              <<
              std::endl;
    return 1;
  }

  int sim_years = atoi(argv[1]);
  std::string params_file = argv[2];
  std::string output_dir = argv[3];

  std::filesystem::path params_abs = std::filesystem::absolute(params_file);
  if (!std::filesystem::exists(params_abs)) {
    std::cout << "Params file '" << params_file << "' does not exist." << std::endl;
    return 1;
  }

  std::filesystem::path output_abs = std::filesystem::absolute(output_dir);
  if (!std::filesystem::exists(output_abs)) {
    if (std::filesystem::create_directory(output_abs)) {
      std::cout << "Created output directory '" << std::string{output_abs} << "'"
                << std::endl;
    } else {
      std::cout << "Failed to create output directory '" << std::string{output_abs} << "'"
                << std::endl;
    }
  } else {
    std::cout << "Writing to existing output directory " << std::string{output_abs} << "'"
              << std::endl;
  }

  if (sim_years > 61) {
    std::cout << "Running to max no of sim years: 61\n" << std::endl;
    sim_years = 61;
  }

  std::vector<int> output_years(sim_years);
  std::iota(output_years.begin(), output_years.end(), 1970);

  const char *n_runs_char = std::getenv("N_RUNS");
  size_t n_runs = 1;
  if (n_runs_char != nullptr) {
    // If we're profiling we want to get accurate info about where time is spent during the
    // main model fit. This runs so quickly though that just going through once won't sample enough
    // times for us to see. And it will sample from the tensor file serialization/deserialization more.
    // So we run the actual model fit multiple times when profiling so the sampler can actually pick
    // up the slow bits.
    n_runs = atoi(n_runs_char);
    std::cout << "Running model fit " << n_runs << " times" << std::endl;
  }

  using LF = leapfrog::Leapfrog<leapfrog::Cpp, double, leapfrog::HivFullAgeStratification>;
  using OP = leapfrog::internal::OwnedParsMixed<double, leapfrog::HivFullAgeStratification>;

  const auto opts = leapfrog::get_opts<double>(10, 30, std::string_view{"midyear"}, 1970, output_years);
  auto owned_pars = OP::parse_pars(params_abs, opts);
  const auto pars = LF::Cfg::get_pars(owned_pars);
  for (size_t i = 0; i < n_runs; ++i) {
    auto state = LF::run_model(pars, opts, output_years);
  }
  std::cout << "Fit complete" << std::endl;

  auto state = LF::run_model(pars, opts, output_years);

  std::filesystem::path output_file = output_abs / "output.h5";
  const H5std_string FILE_NAME(output_file);
  H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);
  file.close();

  LF::Cfg::build_output(0, state, output_file);

  return 0;
}

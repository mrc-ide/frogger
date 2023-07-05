#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

#include <frogger.hpp>
#include <serialize_eigen.hpp>

template<typename T, int rank>
Eigen::TensorMap <Eigen::Tensor<T, rank>> tensor_to_tensor_map(Eigen::Tensor <T, rank> &d) {
  return Eigen::TensorMap < Eigen::Tensor < T, rank >> (d.data(), d.dimensions());
}

void save_output(leapfrog::StateSaver<double, leapfrog::HivAgeStratification::full> &state_saver,
                 std::string &output_path) {
  auto state = state_saver.get_full_state();

  std::filesystem::path out_path(output_path);
  std::filesystem::path total_population_path = out_path / "total_population";
  serialize::serialize_tensor<double, 3>(state.total_population, total_population_path);

  std::filesystem::path births_path = out_path / "births";
  serialize::serialize_tensor<double, 1>(state.births, births_path);

  std::filesystem::path natural_deaths_path = out_path / "natural_deaths";
  serialize::serialize_tensor<double, 3>(state.natural_deaths, natural_deaths_path);

  std::filesystem::path hiv_population_path = out_path / "hiv_population";
  serialize::serialize_tensor<double, 3>(state.hiv_population, hiv_population_path);

  std::filesystem::path hiv_natural_deaths_path = out_path / "hiv_natural_deaths";
  serialize::serialize_tensor<double, 3>(state.hiv_natural_deaths, hiv_natural_deaths_path);

  std::filesystem::path hiv_strat_adult_path = out_path / "hiv_strat_adult";
  serialize::serialize_tensor<double, 4>(state.hiv_strat_adult, hiv_strat_adult_path);

  std::filesystem::path art_strat_adult_path = out_path / "art_strat_adult";
  serialize::serialize_tensor<double, 5>(state.art_strat_adult, art_strat_adult_path);

  std::filesystem::path aids_deaths_no_art_path = out_path / "aids_deaths_no_art";
  serialize::serialize_tensor<double, 4>(state.aids_deaths_no_art, aids_deaths_no_art_path);

  std::filesystem::path infections_path = out_path / "infections";
  serialize::serialize_tensor<double, 3>(state.infections, infections_path);

  std::filesystem::path aids_deaths_art_path = out_path / "aids_deaths_art";
  serialize::serialize_tensor<double, 5>(state.aids_deaths_art, aids_deaths_art_path);

  std::filesystem::path art_initiation_path = out_path / "art_initiation";
  serialize::serialize_tensor<double, 4>(state.art_initiation, art_initiation_path);

  std::filesystem::path hiv_deaths_path = out_path / "hiv_deaths";
  serialize::serialize_tensor<double, 3>(state.hiv_deaths, hiv_deaths_path);
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cout <<
              "Usage: fit_model <sim_years> <hiv_steps_per_year> <intput_dir> <output_dir>" <<
              std::endl;
    return 1;
  }

  int sim_years = atoi(argv[1]);
  int hiv_steps_per_year = atoi(argv[2]);
  std::string input_dir = argv[3];
  std::string output_dir = argv[4];

  std::string input_abs = std::filesystem::absolute(input_dir);
  if (!std::filesystem::exists(input_abs)) {
    std::cout << "Input dir '" << input_dir << "' does not exist." << std::endl;
    return 1;
  }

  std::string output_abs = std::filesystem::absolute(output_dir);
  if (!std::filesystem::exists(output_abs)) {
    if (std::filesystem::create_directory(output_abs)) {
      std::cout << "Created output directory '" << output_abs << "'" << std::endl;
    } else {
      std::cout << "Failed to create output directory '" << output_abs << "'" << std::endl;
    }
  } else {
    std::cout << "Writing to existing output directory " << output_abs << "'" << std::endl;
  }

  if (sim_years > 60) {
    std::cout << "Running to max no of sim years: 60\n" << std::endl;
    sim_years = 60;
  }
  if (hiv_steps_per_year > 10) {
    std::cout << "Running max no of HIV steps per years: 10" << std::endl;
    hiv_steps_per_year = 10;
  }

  // Set working dir to read files
  std::filesystem::path old_dir = std::filesystem::current_path();
  std::filesystem::current_path(input_abs);

  // Only fine-grained ages at first
  const leapfrog::StateSpace ss = leapfrog::StateSpace<leapfrog::HivAgeStratification::full>();


  const leapfrog::Options<double> options = {
      hiv_steps_per_year, // HIV steps per year
      30,                 // Time ART start
      ss.age_groups_hiv,  // Age groups HIV 15+
      1,                  // Scale CD4 mortality
      0.2                 // art_alloc_mxweight
  };

  leapfrog::Tensor1<int> v = serialize::deserialize_tensor<int, 1>(std::string("artcd4elig_idx"));
  for (int i = 0; i <= sim_years; ++i) {
    // 0-based indexing in C++ vs 1-based indexing in R
    v(i) = v[i] - 1;
  }
  const leapfrog::TensorMap1<int> artcd4elig_idx = tensor_to_tensor_map<int, 1>(v);

  leapfrog::Tensor1<double> h(ss.treatment_stages - 1);
  for (int i = 0; i < ss.treatment_stages - 1; ++i) {
    h(i) = 0.5;
  }
  const leapfrog::TensorMap1<double> h_art_stage_dur = tensor_to_tensor_map<double, 1>(h);

  leapfrog::Tensor2<double> base_pop_data = serialize::deserialize_tensor<double, 2>(
      std::string("basepop"));
  const leapfrog::TensorMap2<double> base_pop = tensor_to_tensor_map<double, 2>(base_pop_data);
  leapfrog::Tensor3<double> survival_data = serialize::deserialize_tensor<double, 3>(
      std::string("survival"));
  const leapfrog::TensorMap3<double> survival = tensor_to_tensor_map<double, 3>(survival_data);
  leapfrog::Tensor3<double> net_migration_data = serialize::deserialize_tensor<double, 3>(
      std::string("net_migration"));
  const leapfrog::TensorMap3<double> net_migration = tensor_to_tensor_map<double, 3>(net_migration_data);
  leapfrog::Tensor2<double> age_sex_fertility_ratio_data = serialize::deserialize_tensor<double, 2>(
      std::string("age_sex_fertility_ratio"));
  const leapfrog::TensorMap2<double> age_sex_fertility_ratio = tensor_to_tensor_map<double, 2>(
      age_sex_fertility_ratio_data);
  leapfrog::Tensor2<double> births_sex_prop_data = serialize::deserialize_tensor<double, 2>(
      std::string("births_sex_prop"));
  const leapfrog::TensorMap2<double> births_sex_prop = tensor_to_tensor_map<double, 2>(births_sex_prop_data);
  leapfrog::Tensor1<double> incidence_rate_data = serialize::deserialize_tensor<double, 1>(
      std::string("incidence_rate"));
  const leapfrog::TensorMap1<double> incidence_rate = tensor_to_tensor_map<double, 1>(incidence_rate_data);
  leapfrog::Tensor3<double> incidence_relative_risk_age_data = serialize::deserialize_tensor<double, 3>(
      std::string("incidence_rate_relative_risk_age"));
  const leapfrog::TensorMap3<double> incidence_relative_risk_age = tensor_to_tensor_map<double, 3>(
      incidence_relative_risk_age_data);
  leapfrog::Tensor1<double> incidence_relative_risk_sex_data = serialize::deserialize_tensor<double, 1>(
      std::string("incidence_rate_relative_risk_sex"));
  const leapfrog::TensorMap1<double> incidence_relative_risk_sex = tensor_to_tensor_map<double, 1>(
      incidence_relative_risk_sex_data);
  leapfrog::Tensor3<double> cd4_mortality_data = serialize::deserialize_tensor<double, 3>(
      std::string("cd4_mortality_full"));
  const leapfrog::TensorMap3<double> cd4_mortality = tensor_to_tensor_map<double, 3>(cd4_mortality_data);
  leapfrog::Tensor3<double> cd4_progression_data = serialize::deserialize_tensor<double, 3>(
      std::string("cd4_progression_full"));
  const leapfrog::TensorMap3<double> cd4_progression = tensor_to_tensor_map<double, 3>(cd4_progression_data);
  leapfrog::Tensor3<double> cd4_initdist_data = serialize::deserialize_tensor<double, 3>(
      std::string("cd4_initdist_full"));
  const leapfrog::TensorMap3<double> cd4_initdist = tensor_to_tensor_map<double, 3>(cd4_initdist_data);
  leapfrog::Tensor4<double> art_mortality_data = serialize::deserialize_tensor<double, 4>(
      std::string("art_mortality_full"));
  const leapfrog::TensorMap4<double> art_mortality = tensor_to_tensor_map<double, 4>(art_mortality_data);
  leapfrog::Tensor2<double> artmx_timerr_data = serialize::deserialize_tensor<double, 2>(
      std::string("artmx_timerr"));
  const leapfrog::TensorMap2<double> artmx_timerr = tensor_to_tensor_map<double, 2>(artmx_timerr_data);
  leapfrog::Tensor1<double> art_dropout_data = serialize::deserialize_tensor<double, 1>(
      std::string("art_dropout"));
  const leapfrog::TensorMap1<double> art_dropout = tensor_to_tensor_map<double, 1>(art_dropout_data);
  Eigen::Tensor<double, 2> art15plus_num_data = serialize::deserialize_tensor<double, 2>(
      std::string("art15plus_num"));
  const leapfrog::TensorMap2<double> art15plus_num = tensor_to_tensor_map<double, 2>(art15plus_num_data);
  leapfrog::Tensor2<int> art15plus_isperc_data = serialize::deserialize_tensor<int, 2>(
      std::string("art15plus_isperc"));
  const leapfrog::TensorMap2<int> art15plus_isperc = tensor_to_tensor_map<int, 2>(art15plus_isperc_data);

  const leapfrog::Demography<double> demography = {
      base_pop,
      survival,
      net_migration,
      age_sex_fertility_ratio,
      births_sex_prop
  };

  const leapfrog::Incidence<double> incidence = {
      incidence_rate,
      incidence_relative_risk_age,
      incidence_relative_risk_sex
  };

  const leapfrog::NaturalHistory<double> natural_history = {
      cd4_mortality,
      cd4_progression,
      cd4_initdist
  };

  const leapfrog::Art<double> art = {
      artcd4elig_idx,
      art_mortality,
      artmx_timerr,
      h_art_stage_dur,
      art_dropout,
      art15plus_num,
      art15plus_isperc
  };

  const leapfrog::Parameters<double> params = {options,
                                               demography,
                                               incidence,
                                               natural_history,
                                               art};

  leapfrog::internal::IntermediateData<double, leapfrog::HivAgeStratification::full> intermediate(
      options.age_groups_hiv_15plus);
  leapfrog::State<double, leapfrog::HivAgeStratification::full> state_current;

  std::vector<int> save_steps(61);
  std::iota(save_steps.begin(), save_steps.end(), 0);
  leapfrog::StateSaver<double, leapfrog::HivAgeStratification::full> state_output(sim_years, save_steps);

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

  for (size_t i = 0; i < n_runs; ++i) {
    leapfrog::internal::initialise_model_state<double, leapfrog::HivAgeStratification::full>(params, state_current);
    auto state_next = state_current;

    // Save initial state
    state_output.save_state(state_current, 0);

    // Each time step is mid-point of the year
    for (int step = 1; step <= sim_years; ++step) {
      state_next.reset();
      leapfrog::run_general_pop_demographic_projection<double, leapfrog::HivAgeStratification::full>(step, params,
                                                                                                     state_current,
                                                                                                     state_next,
                                                                                                     intermediate);
      leapfrog::run_hiv_pop_demographic_projection<double, leapfrog::HivAgeStratification::full>(step, params,
                                                                                                 state_current,
                                                                                                 state_next,
                                                                                                 intermediate);
      leapfrog::run_hiv_model_simulation<double, leapfrog::HivAgeStratification::full>(step, params, state_current,
                                                                                       state_next, intermediate);
      state_output.save_state(state_next, step);
      std::swap(state_current, state_next);
      intermediate.reset();
    }
  }
  std::cout << "Fit complete" << std::endl;

  save_output(state_output, output_abs);

  return 0;
}

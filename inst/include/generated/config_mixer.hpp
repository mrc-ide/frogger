#pragma once

#include "config.hpp"

namespace leapfrog {

template<typename ...Ts>
struct ConfigMixer;

template<typename real_type, MV ModelVariant>
struct ConfigMixer<real_type, ModelVariant> {
  struct Pars {};
  static Pars get_pars(const Rcpp::List &data, const Opts<real_type> &options, const int proj_years) {
    Pars p = {}; return p;
  };

  struct Intermediate {
    void reset() {};
  };

  struct State {
    State() {};
    State(const auto initial_state) {};
    void reset() {};
  };

  struct OutputState {
    OutputState(int output_years) {};
    void save_state(const size_t i, const auto &state) {};
  };

  static int get_build_output_size(int prev_size) {
    return prev_size;
  };
  static int build_output(Rcpp::List& ret, Rcpp::CharacterVector& names, int index, const auto& state, const size_t& output_years) {
    return index;
  };
};

template<typename real_type, MV ModelVariant, typename Config, typename ...Ts>
struct ConfigMixer<real_type, ModelVariant, Pair<false, Config>, Ts...> : public ConfigMixer<real_type, ModelVariant, Ts...> {};


template<typename real_type1, MV ModelVariant1, typename ...Ts>
struct ConfigMixer<real_type1, ModelVariant1, Pair<true, DpConfig<real_type1, ModelVariant1>>, Ts...> {
  using real_type = real_type1;
  using ModelVariant = ModelVariant1;
  using CurrConfig = DpConfig<real_type, ModelVariant>;
  using NextConfigMixer = ConfigMixer<real_type, ModelVariant, Ts...>;
  using SS = SSMixed<ModelVariant>;

  struct Pars: public NextConfigMixer::Pars {
    typename CurrConfig::Pars dp;
  };

  static Pars get_pars(const Rcpp::List &data, const Opts<real_type> &options, const int proj_years) {
    Pars p = {
      NextConfigMixer::get_pars(data, options, proj_years),
      CurrConfig::get_pars(data, options, proj_years)
    };
    return p;
  };

  struct Intermediate: public NextConfigMixer::Intermediate {
    typename CurrConfig::Intermediate dp;

    Intermediate():
      NextConfigMixer::Intermediate(),
      dp() {};

    void reset() {
      NextConfigMixer::Intermediate::reset();
      dp.reset();
    };
  };

  struct State: public NextConfigMixer::State {
    typename CurrConfig::State dp;

    State():
      NextConfigMixer::State(),
      dp() {};

    State(const typename CurrConfig::State& initial_state):
      NextConfigMixer::State(initial_state),
      dp(initial_state) {};

    void reset() {
      NextConfigMixer::State::reset();
      dp.reset();
    };
  };

  struct OutputState: public NextConfigMixer::OutputState {
    typename CurrConfig::OutputState dp;

    OutputState(int output_years):
      NextConfigMixer::OutputState(output_years),
      dp(output_years) {};

    void save_state(const size_t i, const State &state) {
      NextConfigMixer::OutputState::save_state(i, state);
      dp.save_state(i, state.dp);
    };
  };

  static int get_build_output_size(int prev_size) {
    int curr_size = NextConfigMixer::get_build_output_size(prev_size);
    return CurrConfig::get_build_output_size(curr_size);
  };

  static int build_output(Rcpp::List& ret, Rcpp::CharacterVector& names, int index, const OutputState& state, const size_t& output_years) {
    int new_index = CurrConfig::build_output(ret, names, index, state.dp, output_years);
    return NextConfigMixer::build_output(ret, names, new_index, state, output_years);
  };

  using Options = Opts<real_type>;
  struct Args {
    int t;
    const Pars& pars;
    const State& state_curr;
    State& state_next;
    Intermediate& intermediate;
    const Options& opts;
  };
};

template<typename real_type1, MV ModelVariant1, typename ...Ts>
struct ConfigMixer<real_type1, ModelVariant1, Pair<true, HaConfig<real_type1, ModelVariant1>>, Ts...> {
  using real_type = real_type1;
  using ModelVariant = ModelVariant1;
  using CurrConfig = HaConfig<real_type, ModelVariant>;
  using NextConfigMixer = ConfigMixer<real_type, ModelVariant, Ts...>;
  using SS = SSMixed<ModelVariant>;

  struct Pars: public NextConfigMixer::Pars {
    typename CurrConfig::Pars ha;
  };

  static Pars get_pars(const Rcpp::List &data, const Opts<real_type> &options, const int proj_years) {
    Pars p = {
      NextConfigMixer::get_pars(data, options, proj_years),
      CurrConfig::get_pars(data, options, proj_years)
    };
    return p;
  };

  struct Intermediate: public NextConfigMixer::Intermediate {
    typename CurrConfig::Intermediate ha;

    Intermediate():
      NextConfigMixer::Intermediate(),
      ha() {};

    void reset() {
      NextConfigMixer::Intermediate::reset();
      ha.reset();
    };
  };

  struct State: public NextConfigMixer::State {
    typename CurrConfig::State ha;

    State():
      NextConfigMixer::State(),
      ha() {};

    State(const typename CurrConfig::State& initial_state):
      NextConfigMixer::State(initial_state),
      ha(initial_state) {};

    void reset() {
      NextConfigMixer::State::reset();
      ha.reset();
    };
  };

  struct OutputState: public NextConfigMixer::OutputState {
    typename CurrConfig::OutputState ha;

    OutputState(int output_years):
      NextConfigMixer::OutputState(output_years),
      ha(output_years) {};

    void save_state(const size_t i, const State &state) {
      NextConfigMixer::OutputState::save_state(i, state);
      ha.save_state(i, state.ha);
    };
  };

  static int get_build_output_size(int prev_size) {
    int curr_size = NextConfigMixer::get_build_output_size(prev_size);
    return CurrConfig::get_build_output_size(curr_size);
  };

  static int build_output(Rcpp::List& ret, Rcpp::CharacterVector& names, int index, const OutputState& state, const size_t& output_years) {
    int new_index = CurrConfig::build_output(ret, names, index, state.ha, output_years);
    return NextConfigMixer::build_output(ret, names, new_index, state, output_years);
  };

  using Options = Opts<real_type>;
  struct Args {
    int t;
    const Pars& pars;
    const State& state_curr;
    State& state_next;
    Intermediate& intermediate;
    const Options& opts;
  };
};

template<typename real_type1, MV ModelVariant1, typename ...Ts>
struct ConfigMixer<real_type1, ModelVariant1, Pair<true, HcConfig<real_type1, ModelVariant1>>, Ts...> {
  using real_type = real_type1;
  using ModelVariant = ModelVariant1;
  using CurrConfig = HcConfig<real_type, ModelVariant>;
  using NextConfigMixer = ConfigMixer<real_type, ModelVariant, Ts...>;
  using SS = SSMixed<ModelVariant>;

  struct Pars: public NextConfigMixer::Pars {
    typename CurrConfig::Pars hc;
  };

  static Pars get_pars(const Rcpp::List &data, const Opts<real_type> &options, const int proj_years) {
    Pars p = {
      NextConfigMixer::get_pars(data, options, proj_years),
      CurrConfig::get_pars(data, options, proj_years)
    };
    return p;
  };

  struct Intermediate: public NextConfigMixer::Intermediate {
    typename CurrConfig::Intermediate hc;

    Intermediate():
      NextConfigMixer::Intermediate(),
      hc() {};

    void reset() {
      NextConfigMixer::Intermediate::reset();
      hc.reset();
    };
  };

  struct State: public NextConfigMixer::State {
    typename CurrConfig::State hc;

    State():
      NextConfigMixer::State(),
      hc() {};

    State(const typename CurrConfig::State& initial_state):
      NextConfigMixer::State(initial_state),
      hc(initial_state) {};

    void reset() {
      NextConfigMixer::State::reset();
      hc.reset();
    };
  };

  struct OutputState: public NextConfigMixer::OutputState {
    typename CurrConfig::OutputState hc;

    OutputState(int output_years):
      NextConfigMixer::OutputState(output_years),
      hc(output_years) {};

    void save_state(const size_t i, const State &state) {
      NextConfigMixer::OutputState::save_state(i, state);
      hc.save_state(i, state.hc);
    };
  };

  static int get_build_output_size(int prev_size) {
    int curr_size = NextConfigMixer::get_build_output_size(prev_size);
    return CurrConfig::get_build_output_size(curr_size);
  };

  static int build_output(Rcpp::List& ret, Rcpp::CharacterVector& names, int index, const OutputState& state, const size_t& output_years) {
    int new_index = CurrConfig::build_output(ret, names, index, state.hc, output_years);
    return NextConfigMixer::build_output(ret, names, new_index, state, output_years);
  };

  using Options = Opts<real_type>;
  struct Args {
    int t;
    const Pars& pars;
    const State& state_curr;
    State& state_next;
    Intermediate& intermediate;
    const Options& opts;
  };
};

template<typename real_type, MV ModelVariant>
using ConfigMixed = ConfigMixer<
  real_type, ModelVariant,
  Pair<ModelVariant::run_demographic_projection, DpConfig<real_type, ModelVariant>>,
  Pair<ModelVariant::run_hiv_simulation, HaConfig<real_type, ModelVariant>>,
  Pair<ModelVariant::run_child_model, HcConfig<real_type, ModelVariant>>
>;

}

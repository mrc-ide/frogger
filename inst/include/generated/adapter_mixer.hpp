#pragma once

#include "config.hpp"
#include "config_mixer.hpp"
#include "../options.hpp"

namespace leapfrog {

namespace internal {

template<bool enable, typename ConfigMixin, typename AdapterMixin>
struct Triple;

template<typename TripleT>
struct TripleToPair;

template<bool enable, typename ConfigMixin, typename AdapterMixin>
struct TripleToPair<Triple<enable, ConfigMixin, AdapterMixin>> {
  using type = Pair<enable, ConfigMixin>;
};

template<typename ...Ts>
struct AdapterMixer;

template<typename real_type, MV ModelVariant>
struct AdapterMixer<real_type, ModelVariant> {
  using Config = ConfigMixer<real_type, ModelVariant>;

  template<typename... Args>
  static typename Config::Pars get_pars(Args&&... args) {
    typename Config::Pars p = {}; return p;
  }

  template<typename... Args>
  static int build_output(int index, const auto& state, Args&&... args) {
    return index;
  }
};

template<typename real_type, MV ModelVariant, typename ConfigMixin, typename AdapterMixin, typename ...Ts>
struct AdapterMixer<real_type, ModelVariant, Triple<false, ConfigMixin, AdapterMixin>, Ts...> : public AdapterMixer<real_type, ModelVariant, Ts...> {};

template<typename real_type1, MV ModelVariant1, typename AdapterMixin, typename ...Ts>
struct AdapterMixer<real_type1, ModelVariant1, Triple<true, DpConfig<real_type1, ModelVariant1>, AdapterMixin>, Ts...> {
  using real_type = real_type1;
  using ModelVariant = ModelVariant1;
  using CurrAdapter = AdapterMixin;
  using NextAdapterMixer = AdapterMixer<real_type, ModelVariant, Ts...>;
  using Config = ConfigMixer<real_type, ModelVariant, Pair<true, DpConfig<real_type, ModelVariant>>, typename TripleToPair<Ts>::type...>;

  template<typename... Args>
  static typename Config::Pars get_pars(Args&&... args) {
    typename Config::Pars p = {
      NextAdapterMixer::get_pars(std::forward<Args>(args)...),
      CurrAdapter::get_pars(std::forward<Args>(args)...)
    };
    return p;
  }

  template<typename... Args>
  static int build_output(int index, const auto& state, Args&&... args) {
    int new_index = CurrAdapter::build_output(index, state.dp, std::forward<Args>(args)...);
    return NextAdapterMixer::build_output(new_index, state, std::forward<Args>(args)...);
  }
};

template<typename real_type1, MV ModelVariant1, typename AdapterMixin, typename ...Ts>
struct AdapterMixer<real_type1, ModelVariant1, Triple<true, HaConfig<real_type1, ModelVariant1>, AdapterMixin>, Ts...> {
  using real_type = real_type1;
  using ModelVariant = ModelVariant1;
  using CurrAdapter = AdapterMixin;
  using NextAdapterMixer = AdapterMixer<real_type, ModelVariant, Ts...>;
  using Config = ConfigMixer<real_type, ModelVariant, Pair<true, HaConfig<real_type, ModelVariant>>, typename TripleToPair<Ts>::type...>;

  template<typename... Args>
  static typename Config::Pars get_pars(Args&&... args) {
    typename Config::Pars p = {
      NextAdapterMixer::get_pars(std::forward<Args>(args)...),
      CurrAdapter::get_pars(std::forward<Args>(args)...)
    };
    return p;
  }

  template<typename... Args>
  static int build_output(int index, const auto& state, Args&&... args) {
    int new_index = CurrAdapter::build_output(index, state.ha, std::forward<Args>(args)...);
    return NextAdapterMixer::build_output(new_index, state, std::forward<Args>(args)...);
  }
};

template<typename real_type1, MV ModelVariant1, typename AdapterMixin, typename ...Ts>
struct AdapterMixer<real_type1, ModelVariant1, Triple<true, HcConfig<real_type1, ModelVariant1>, AdapterMixin>, Ts...> {
  using real_type = real_type1;
  using ModelVariant = ModelVariant1;
  using CurrAdapter = AdapterMixin;
  using NextAdapterMixer = AdapterMixer<real_type, ModelVariant, Ts...>;
  using Config = ConfigMixer<real_type, ModelVariant, Pair<true, HcConfig<real_type, ModelVariant>>, typename TripleToPair<Ts>::type...>;

  template<typename... Args>
  static typename Config::Pars get_pars(Args&&... args) {
    typename Config::Pars p = {
      NextAdapterMixer::get_pars(std::forward<Args>(args)...),
      CurrAdapter::get_pars(std::forward<Args>(args)...)
    };
    return p;
  }

  template<typename... Args>
  static int build_output(int index, const auto& state, Args&&... args) {
    int new_index = CurrAdapter::build_output(index, state.hc, std::forward<Args>(args)...);
    return NextAdapterMixer::build_output(new_index, state, std::forward<Args>(args)...);
  }
};

}
}

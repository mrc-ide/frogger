#pragma once

#include "concepts.hpp"

namespace leapfrog {


template<MV ModelVariant>
struct DpSS {
  static constexpr int NS = 2;
  static constexpr int pAG = 81;
};


template<MV ModelVariant>
struct HaSS {
  static constexpr std::array<int, 66> hAG_span = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  static constexpr int hDS = 7;
  static constexpr int hTS = 3;
  static constexpr int hAG = 66;
};


template<MV ModelVariant>
requires(ModelVariant::use_coarse_stratification)
struct HaSS<ModelVariant> {
  static constexpr std::array<int, 9> hAG_span = { 2, 3, 5, 5, 5, 5, 5, 5, 31 };
  static constexpr int hDS = 7;
  static constexpr int hTS = 3;
  static constexpr int hAG = 9;
};


template<MV ModelVariant>
struct HcSS {
  static constexpr int hc1DS = 7;
  static constexpr int hc2DS = 6;
  static constexpr int hc1_ageend = 4;
  static constexpr int hc2_agestart = 5;
  static constexpr int hc1AG = 5;
  static constexpr int hc1AG_c = 2;
  static constexpr int hc2AG = 10;
  static constexpr int hc2AG_c = 1;
  static constexpr int hcAG_end = 15;
  static constexpr int hcTT = 4;
  static constexpr int hPS = 7;
  static constexpr int hPS_dropout = 6;
  static constexpr int hVT = 2;
  static constexpr int hBF = 18;
  static constexpr int hBF_coarse = 4;
  static constexpr int hcAG_coarse = 4;
  static constexpr int hAB_ind = 2;
};


struct BaseSS {
  static constexpr int MALE = 0;
  static constexpr int FEMALE = 1;
  static constexpr int ART0MOS = 0;
  static constexpr int PROJPERIOD_CALENDAR = 0;
  static constexpr int PROJPERIOD_MIDYEAR = 1;
};

}

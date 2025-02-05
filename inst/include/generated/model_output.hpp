// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `model_output.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include <Rcpp.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "../state_saver.hpp"
#include "../r_utils.hpp"

template<typename ModelVariant, typename real_type>
Rcpp::List build_r_output(const leapfrog::OutputState<ModelVariant, real_type> &state,
                          const std::vector<int> save_steps) {
  size_t output_years = save_steps.size();
  constexpr auto ss = leapfrog::StateSpace<ModelVariant>();
  constexpr auto dp = ss.dp;
  Rcpp::NumericVector r_p_total_pop(dp.pAG * dp.NS * output_years);
  Rcpp::NumericVector r_births(output_years);
  Rcpp::NumericVector r_p_total_pop_natural_deaths(dp.pAG * dp.NS * output_years);
  r_p_total_pop.attr("dim") = Rcpp::NumericVector::create(dp.pAG, dp.NS, output_years);
  r_births.attr("dim") = Rcpp::NumericVector::create(output_years);
  r_p_total_pop_natural_deaths.attr("dim") = Rcpp::NumericVector::create(dp.pAG, dp.NS, output_years);
  std::copy_n(state.dp.p_total_pop.data(), state.dp.p_total_pop.size(), REAL(r_p_total_pop));
  std::copy_n(state.dp.births.data(), state.dp.births.size(), REAL(r_births));
  std::copy_n(state.dp.p_total_pop_natural_deaths.data(), state.dp.p_total_pop_natural_deaths.size(), REAL(r_p_total_pop_natural_deaths));

  Rcpp::List ret(3);
  Rcpp::CharacterVector names(3);
  names[0] = "p_total_pop";
  ret[0] = r_p_total_pop;
  names[1] = "births";
  ret[1] = r_births;
  names[2] = "p_total_pop_natural_deaths";
  ret[2] = r_p_total_pop_natural_deaths;
  ret.attr("names") = names;

  if constexpr (ModelVariant::run_hiv_simulation) {
    constexpr auto dp = ss.dp;
    constexpr auto hiv = ss.hiv;
    Rcpp::NumericVector r_p_hiv_pop(dp.pAG * dp.NS * output_years);
    Rcpp::NumericVector r_p_hiv_pop_natural_deaths(dp.pAG * dp.NS * output_years);
    Rcpp::NumericVector r_h_hiv_adult(hiv.hDS * hiv.hAG * dp.NS * output_years);
    Rcpp::NumericVector r_h_art_adult(hiv.hTS * hiv.hDS * hiv.hAG * dp.NS * output_years);
    Rcpp::NumericVector r_h_hiv_deaths_no_art(hiv.hDS * hiv.hAG * dp.NS * output_years);
    Rcpp::NumericVector r_p_infections(dp.pAG * dp.NS * output_years);
    Rcpp::NumericVector r_h_hiv_deaths_art(hiv.hTS * hiv.hDS * hiv.hAG * dp.NS * output_years);
    Rcpp::NumericVector r_h_art_initiation(hiv.hDS * hiv.hAG * dp.NS * output_years);
    Rcpp::NumericVector r_p_hiv_deaths(dp.pAG * dp.NS * output_years);
    r_p_hiv_pop.attr("dim") = Rcpp::NumericVector::create(dp.pAG, dp.NS, output_years);
    r_p_hiv_pop_natural_deaths.attr("dim") = Rcpp::NumericVector::create(dp.pAG, dp.NS, output_years);
    r_h_hiv_adult.attr("dim") = Rcpp::NumericVector::create(hiv.hDS, hiv.hAG, dp.NS, output_years);
    r_h_art_adult.attr("dim") = Rcpp::NumericVector::create(hiv.hTS, hiv.hDS, hiv.hAG, dp.NS, output_years);
    r_h_hiv_deaths_no_art.attr("dim") = Rcpp::NumericVector::create(hiv.hDS, hiv.hAG, dp.NS, output_years);
    r_p_infections.attr("dim") = Rcpp::NumericVector::create(dp.pAG, dp.NS, output_years);
    r_h_hiv_deaths_art.attr("dim") = Rcpp::NumericVector::create(hiv.hTS, hiv.hDS, hiv.hAG, dp.NS, output_years);
    r_h_art_initiation.attr("dim") = Rcpp::NumericVector::create(hiv.hDS, hiv.hAG, dp.NS, output_years);
    r_p_hiv_deaths.attr("dim") = Rcpp::NumericVector::create(dp.pAG, dp.NS, output_years);
    std::copy_n(state.hiv.p_hiv_pop.data(), state.hiv.p_hiv_pop.size(), REAL(r_p_hiv_pop));
    std::copy_n(state.hiv.p_hiv_pop_natural_deaths.data(), state.hiv.p_hiv_pop_natural_deaths.size(), REAL(r_p_hiv_pop_natural_deaths));
    std::copy_n(state.hiv.h_hiv_adult.data(), state.hiv.h_hiv_adult.size(), REAL(r_h_hiv_adult));
    std::copy_n(state.hiv.h_art_adult.data(), state.hiv.h_art_adult.size(), REAL(r_h_art_adult));
    std::copy_n(state.hiv.h_hiv_deaths_no_art.data(), state.hiv.h_hiv_deaths_no_art.size(), REAL(r_h_hiv_deaths_no_art));
    std::copy_n(state.hiv.p_infections.data(), state.hiv.p_infections.size(), REAL(r_p_infections));
    std::copy_n(state.hiv.h_hiv_deaths_art.data(), state.hiv.h_hiv_deaths_art.size(), REAL(r_h_hiv_deaths_art));
    std::copy_n(state.hiv.h_art_initiation.data(), state.hiv.h_art_initiation.size(), REAL(r_h_art_initiation));
    std::copy_n(state.hiv.p_hiv_deaths.data(), state.hiv.p_hiv_deaths.size(), REAL(r_p_hiv_deaths));
    ret.push_back(r_p_hiv_pop, "p_hiv_pop");
    ret.push_back(r_p_hiv_pop_natural_deaths, "p_hiv_pop_natural_deaths");
    ret.push_back(r_h_hiv_adult, "h_hiv_adult");
    ret.push_back(r_h_art_adult, "h_art_adult");
    ret.push_back(r_h_hiv_deaths_no_art, "h_hiv_deaths_no_art");
    ret.push_back(r_p_infections, "p_infections");
    ret.push_back(r_h_hiv_deaths_art, "h_hiv_deaths_art");
    ret.push_back(r_h_art_initiation, "h_art_initiation");
    ret.push_back(r_p_hiv_deaths, "p_hiv_deaths");
  }
  if constexpr (ModelVariant::run_child_model) {
    constexpr auto children = ss.children;
    constexpr auto hiv = ss.hiv;
    constexpr auto dp = ss.dp;
    Rcpp::NumericVector r_hc1_hiv_pop(children.hc1DS * children.hcTT * children.hc1AG * dp.NS * output_years);
    Rcpp::NumericVector r_hc2_hiv_pop(children.hc2DS * children.hcTT * children.hc2AG * dp.NS * output_years);
    Rcpp::NumericVector r_hc1_art_pop(hiv.hTS * children.hc1DS * children.hc1AG * dp.NS * output_years);
    Rcpp::NumericVector r_hc2_art_pop(hiv.hTS * children.hc2DS * children.hc2AG * dp.NS * output_years);
    Rcpp::NumericVector r_hc1_noart_aids_deaths(children.hc1DS * children.hcTT * children.hc1AG * dp.NS * output_years);
    Rcpp::NumericVector r_hc2_noart_aids_deaths(children.hc2DS * children.hcTT * children.hc2AG * dp.NS * output_years);
    Rcpp::NumericVector r_hc1_art_aids_deaths(hiv.hTS * children.hc1DS * children.hc1AG * dp.NS * output_years);
    Rcpp::NumericVector r_hc2_art_aids_deaths(hiv.hTS * children.hc2DS * children.hc2AG * dp.NS * output_years);
    Rcpp::NumericVector r_hiv_births(output_years);
    Rcpp::NumericVector r_hc_art_init(children.hcAG_coarse * output_years);
    Rcpp::NumericVector r_hc_art_need_init(children.hc1DS * children.hcTT * children.hcAG_end * dp.NS * output_years);
    Rcpp::NumericVector r_ctx_need(output_years);
    Rcpp::NumericVector r_ctx_mean(output_years);
    r_hc1_hiv_pop.attr("dim") = Rcpp::NumericVector::create(children.hc1DS, children.hcTT, children.hc1AG, dp.NS, output_years);
    r_hc2_hiv_pop.attr("dim") = Rcpp::NumericVector::create(children.hc2DS, children.hcTT, children.hc2AG, dp.NS, output_years);
    r_hc1_art_pop.attr("dim") = Rcpp::NumericVector::create(hiv.hTS, children.hc1DS, children.hc1AG, dp.NS, output_years);
    r_hc2_art_pop.attr("dim") = Rcpp::NumericVector::create(hiv.hTS, children.hc2DS, children.hc2AG, dp.NS, output_years);
    r_hc1_noart_aids_deaths.attr("dim") = Rcpp::NumericVector::create(children.hc1DS, children.hcTT, children.hc1AG, dp.NS, output_years);
    r_hc2_noart_aids_deaths.attr("dim") = Rcpp::NumericVector::create(children.hc2DS, children.hcTT, children.hc2AG, dp.NS, output_years);
    r_hc1_art_aids_deaths.attr("dim") = Rcpp::NumericVector::create(hiv.hTS, children.hc1DS, children.hc1AG, dp.NS, output_years);
    r_hc2_art_aids_deaths.attr("dim") = Rcpp::NumericVector::create(hiv.hTS, children.hc2DS, children.hc2AG, dp.NS, output_years);
    r_hiv_births.attr("dim") = Rcpp::NumericVector::create(output_years);
    r_hc_art_init.attr("dim") = Rcpp::NumericVector::create(children.hcAG_coarse, output_years);
    r_hc_art_need_init.attr("dim") = Rcpp::NumericVector::create(children.hc1DS, children.hcTT, children.hcAG_end, dp.NS, output_years);
    r_ctx_need.attr("dim") = Rcpp::NumericVector::create(output_years);
    r_ctx_mean.attr("dim") = Rcpp::NumericVector::create(output_years);
    std::copy_n(state.children.hc1_hiv_pop.data(), state.children.hc1_hiv_pop.size(), REAL(r_hc1_hiv_pop));
    std::copy_n(state.children.hc2_hiv_pop.data(), state.children.hc2_hiv_pop.size(), REAL(r_hc2_hiv_pop));
    std::copy_n(state.children.hc1_art_pop.data(), state.children.hc1_art_pop.size(), REAL(r_hc1_art_pop));
    std::copy_n(state.children.hc2_art_pop.data(), state.children.hc2_art_pop.size(), REAL(r_hc2_art_pop));
    std::copy_n(state.children.hc1_noart_aids_deaths.data(), state.children.hc1_noart_aids_deaths.size(), REAL(r_hc1_noart_aids_deaths));
    std::copy_n(state.children.hc2_noart_aids_deaths.data(), state.children.hc2_noart_aids_deaths.size(), REAL(r_hc2_noart_aids_deaths));
    std::copy_n(state.children.hc1_art_aids_deaths.data(), state.children.hc1_art_aids_deaths.size(), REAL(r_hc1_art_aids_deaths));
    std::copy_n(state.children.hc2_art_aids_deaths.data(), state.children.hc2_art_aids_deaths.size(), REAL(r_hc2_art_aids_deaths));
    std::copy_n(state.children.hiv_births.data(), state.children.hiv_births.size(), REAL(r_hiv_births));
    std::copy_n(state.children.hc_art_init.data(), state.children.hc_art_init.size(), REAL(r_hc_art_init));
    std::copy_n(state.children.hc_art_need_init.data(), state.children.hc_art_need_init.size(), REAL(r_hc_art_need_init));
    std::copy_n(state.children.ctx_need.data(), state.children.ctx_need.size(), REAL(r_ctx_need));
    std::copy_n(state.children.ctx_mean.data(), state.children.ctx_mean.size(), REAL(r_ctx_mean));
    ret.push_back(r_hc1_hiv_pop, "hc1_hiv_pop");
    ret.push_back(r_hc2_hiv_pop, "hc2_hiv_pop");
    ret.push_back(r_hc1_art_pop, "hc1_art_pop");
    ret.push_back(r_hc2_art_pop, "hc2_art_pop");
    ret.push_back(r_hc1_noart_aids_deaths, "hc1_noart_aids_deaths");
    ret.push_back(r_hc2_noart_aids_deaths, "hc2_noart_aids_deaths");
    ret.push_back(r_hc1_art_aids_deaths, "hc1_art_aids_deaths");
    ret.push_back(r_hc2_art_aids_deaths, "hc2_art_aids_deaths");
    ret.push_back(r_hiv_births, "hiv_births");
    ret.push_back(r_hc_art_init, "hc_art_init");
    ret.push_back(r_hc_art_need_init, "hc_art_need_init");
    ret.push_back(r_ctx_need, "ctx_need");
    ret.push_back(r_ctx_mean, "ctx_mean");
  }

  return ret;
}

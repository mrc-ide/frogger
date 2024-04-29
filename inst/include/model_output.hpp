// Generated by frogger: do not edit by hand
// This file is automatically generated. Do not edit this file. If you want to make changes
// edit `model_output.hpp.in` and run `./scripts/generate` to regenerate.

#pragma once

#include <Rcpp.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "state_saver.hpp"
#include "r_utils.hpp"

template<typename ModelVariant, typename real_type>
Rcpp::List build_r_output(const leapfrog::OutputState<ModelVariant, real_type> &state,
                          const std::vector<int> save_steps) {
  size_t output_years = save_steps.size();
  constexpr auto ss = leapfrog::StateSpace<ModelVariant>();
  constexpr auto base = ss.base;
  Rcpp::NumericVector r_p_total_pop(base.pAG * base.NS * output_years);
  Rcpp::NumericVector r_births(output_years);
  Rcpp::NumericVector r_p_total_pop_natural_deaths(base.pAG * base.NS * output_years);
  Rcpp::NumericVector r_p_hiv_pop(base.pAG * base.NS * output_years);
  Rcpp::NumericVector r_p_hiv_pop_natural_deaths(base.pAG * base.NS * output_years);
  Rcpp::NumericVector r_h_hiv_adult(base.hDS * base.hAG * base.NS * output_years);
  Rcpp::NumericVector r_h_art_adult(base.hTS * base.hDS * base.hAG * base.NS * output_years);
  Rcpp::NumericVector r_h_hiv_deaths_no_art(base.hDS * base.hAG * base.NS * output_years);
  Rcpp::NumericVector r_p_infections(base.pAG * base.NS * output_years);
  Rcpp::NumericVector r_h_hiv_deaths_art(base.hTS * base.hDS * base.hAG * base.NS * output_years);
  Rcpp::NumericVector r_h_art_initiation(base.hDS * base.hAG * base.NS * output_years);
  Rcpp::NumericVector r_p_hiv_deaths(base.pAG * base.NS * output_years);
  r_p_total_pop.attr("dim") = Rcpp::NumericVector::create(base.pAG, base.NS, output_years);
  r_births.attr("dim") = Rcpp::NumericVector::create(output_years);
  r_p_total_pop_natural_deaths.attr("dim") = Rcpp::NumericVector::create(base.pAG, base.NS, output_years);
  r_p_hiv_pop.attr("dim") = Rcpp::NumericVector::create(base.pAG, base.NS, output_years);
  r_p_hiv_pop_natural_deaths.attr("dim") = Rcpp::NumericVector::create(base.pAG, base.NS, output_years);
  r_h_hiv_adult.attr("dim") = Rcpp::NumericVector::create(base.hDS, base.hAG, base.NS, output_years);
  r_h_art_adult.attr("dim") = Rcpp::NumericVector::create(base.hTS, base.hDS, base.hAG, base.NS, output_years);
  r_h_hiv_deaths_no_art.attr("dim") = Rcpp::NumericVector::create(base.hDS, base.hAG, base.NS, output_years);
  r_p_infections.attr("dim") = Rcpp::NumericVector::create(base.pAG, base.NS, output_years);
  r_h_hiv_deaths_art.attr("dim") = Rcpp::NumericVector::create(base.hTS, base.hDS, base.hAG, base.NS, output_years);
  r_h_art_initiation.attr("dim") = Rcpp::NumericVector::create(base.hDS, base.hAG, base.NS, output_years);
  r_p_hiv_deaths.attr("dim") = Rcpp::NumericVector::create(base.pAG, base.NS, output_years);
  std::copy_n(state.base.p_total_pop.data(), state.base.p_total_pop.size(), REAL(r_p_total_pop));
  std::copy_n(state.base.births.data(), state.base.births.size(), REAL(r_births));
  std::copy_n(state.base.p_total_pop_natural_deaths.data(), state.base.p_total_pop_natural_deaths.size(), REAL(r_p_total_pop_natural_deaths));
  std::copy_n(state.base.p_hiv_pop.data(), state.base.p_hiv_pop.size(), REAL(r_p_hiv_pop));
  std::copy_n(state.base.p_hiv_pop_natural_deaths.data(), state.base.p_hiv_pop_natural_deaths.size(), REAL(r_p_hiv_pop_natural_deaths));
  std::copy_n(state.base.h_hiv_adult.data(), state.base.h_hiv_adult.size(), REAL(r_h_hiv_adult));
  std::copy_n(state.base.h_art_adult.data(), state.base.h_art_adult.size(), REAL(r_h_art_adult));
  std::copy_n(state.base.h_hiv_deaths_no_art.data(), state.base.h_hiv_deaths_no_art.size(), REAL(r_h_hiv_deaths_no_art));
  std::copy_n(state.base.p_infections.data(), state.base.p_infections.size(), REAL(r_p_infections));
  std::copy_n(state.base.h_hiv_deaths_art.data(), state.base.h_hiv_deaths_art.size(), REAL(r_h_hiv_deaths_art));
  std::copy_n(state.base.h_art_initiation.data(), state.base.h_art_initiation.size(), REAL(r_h_art_initiation));
  std::copy_n(state.base.p_hiv_deaths.data(), state.base.p_hiv_deaths.size(), REAL(r_p_hiv_deaths));

  Rcpp::List ret(12);
  Rcpp::CharacterVector names(12);
  names[0] = "p_total_pop";
  ret[0] = r_p_total_pop;
  names[1] = "births";
  ret[1] = r_births;
  names[2] = "p_total_pop_natural_deaths";
  ret[2] = r_p_total_pop_natural_deaths;
  names[3] = "p_hiv_pop";
  ret[3] = r_p_hiv_pop;
  names[4] = "p_hiv_pop_natural_deaths";
  ret[4] = r_p_hiv_pop_natural_deaths;
  names[5] = "h_hiv_adult";
  ret[5] = r_h_hiv_adult;
  names[6] = "h_art_adult";
  ret[6] = r_h_art_adult;
  names[7] = "h_hiv_deaths_no_art";
  ret[7] = r_h_hiv_deaths_no_art;
  names[8] = "p_infections";
  ret[8] = r_p_infections;
  names[9] = "h_hiv_deaths_art";
  ret[9] = r_h_hiv_deaths_art;
  names[10] = "h_art_initiation";
  ret[10] = r_h_art_initiation;
  names[11] = "p_hiv_deaths";
  ret[11] = r_p_hiv_deaths;
  ret.attr("names") = names;

  if constexpr (ModelVariant::run_child_model) {
    constexpr auto children = ss.children;
    constexpr auto base = ss.base;
    Rcpp::NumericVector r_hc1_hiv_pop(children.hc1DS * children.hcTT * children.hc1AG * base.NS * output_years);
    Rcpp::NumericVector r_hc2_hiv_pop(children.hc2DS * children.hcTT * children.hc2AG * base.NS * output_years);
    Rcpp::NumericVector r_hc1_art_pop(base.hTS * children.hc1DS * children.hc1AG * base.NS * output_years);
    Rcpp::NumericVector r_hc2_art_pop(base.hTS * children.hc2DS * children.hc2AG * base.NS * output_years);
    Rcpp::NumericVector r_hc1_noart_aids_deaths(children.hc1DS * children.hcTT * children.hc1AG * base.NS * output_years);
    Rcpp::NumericVector r_hc2_noart_aids_deaths(children.hc2DS * children.hcTT * children.hc2AG * base.NS * output_years);
    Rcpp::NumericVector r_hc1_art_aids_deaths(base.hTS * children.hc1DS * children.hc1AG * base.NS * output_years);
    Rcpp::NumericVector r_hc2_art_aids_deaths(base.hTS * children.hc2DS * children.hc2AG * base.NS * output_years);
    Rcpp::NumericVector r_hc_art_num(4 * output_years);
    Rcpp::NumericVector r_hiv_births(output_years);
    Rcpp::NumericVector r_hc_art_total(4 * output_years);
    Rcpp::NumericVector r_hc_art_init(4 * output_years);
    Rcpp::NumericVector r_hc_art_need_init(children.hc1DS * children.hcTT * 15 * base.NS * output_years);
    Rcpp::NumericVector r_ctx_need(output_years);
    r_hc1_hiv_pop.attr("dim") = Rcpp::NumericVector::create(children.hc1DS, children.hcTT, children.hc1AG, base.NS, output_years);
    r_hc2_hiv_pop.attr("dim") = Rcpp::NumericVector::create(children.hc2DS, children.hcTT, children.hc2AG, base.NS, output_years);
    r_hc1_art_pop.attr("dim") = Rcpp::NumericVector::create(base.hTS, children.hc1DS, children.hc1AG, base.NS, output_years);
    r_hc2_art_pop.attr("dim") = Rcpp::NumericVector::create(base.hTS, children.hc2DS, children.hc2AG, base.NS, output_years);
    r_hc1_noart_aids_deaths.attr("dim") = Rcpp::NumericVector::create(children.hc1DS, children.hcTT, children.hc1AG, base.NS, output_years);
    r_hc2_noart_aids_deaths.attr("dim") = Rcpp::NumericVector::create(children.hc2DS, children.hcTT, children.hc2AG, base.NS, output_years);
    r_hc1_art_aids_deaths.attr("dim") = Rcpp::NumericVector::create(base.hTS, children.hc1DS, children.hc1AG, base.NS, output_years);
    r_hc2_art_aids_deaths.attr("dim") = Rcpp::NumericVector::create(base.hTS, children.hc2DS, children.hc2AG, base.NS, output_years);
    r_hc_art_num.attr("dim") = Rcpp::NumericVector::create(4, output_years);
    r_hiv_births.attr("dim") = Rcpp::NumericVector::create(output_years);
    r_hc_art_total.attr("dim") = Rcpp::NumericVector::create(4, output_years);
    r_hc_art_init.attr("dim") = Rcpp::NumericVector::create(4, output_years);
    r_hc_art_need_init.attr("dim") = Rcpp::NumericVector::create(children.hc1DS, children.hcTT, 15, base.NS, output_years);
    r_ctx_need.attr("dim") = Rcpp::NumericVector::create(output_years);
    std::copy_n(state.children.hc1_hiv_pop.data(), state.children.hc1_hiv_pop.size(), REAL(r_hc1_hiv_pop));
    std::copy_n(state.children.hc2_hiv_pop.data(), state.children.hc2_hiv_pop.size(), REAL(r_hc2_hiv_pop));
    std::copy_n(state.children.hc1_art_pop.data(), state.children.hc1_art_pop.size(), REAL(r_hc1_art_pop));
    std::copy_n(state.children.hc2_art_pop.data(), state.children.hc2_art_pop.size(), REAL(r_hc2_art_pop));
    std::copy_n(state.children.hc1_noart_aids_deaths.data(), state.children.hc1_noart_aids_deaths.size(), REAL(r_hc1_noart_aids_deaths));
    std::copy_n(state.children.hc2_noart_aids_deaths.data(), state.children.hc2_noart_aids_deaths.size(), REAL(r_hc2_noart_aids_deaths));
    std::copy_n(state.children.hc1_art_aids_deaths.data(), state.children.hc1_art_aids_deaths.size(), REAL(r_hc1_art_aids_deaths));
    std::copy_n(state.children.hc2_art_aids_deaths.data(), state.children.hc2_art_aids_deaths.size(), REAL(r_hc2_art_aids_deaths));
    std::copy_n(state.children.hc_art_num.data(), state.children.hc_art_num.size(), REAL(r_hc_art_num));
    std::copy_n(state.children.hiv_births.data(), state.children.hiv_births.size(), REAL(r_hiv_births));
    std::copy_n(state.children.hc_art_total.data(), state.children.hc_art_total.size(), REAL(r_hc_art_total));
    std::copy_n(state.children.hc_art_init.data(), state.children.hc_art_init.size(), REAL(r_hc_art_init));
    std::copy_n(state.children.hc_art_need_init.data(), state.children.hc_art_need_init.size(), REAL(r_hc_art_need_init));
    std::copy_n(state.children.ctx_need.data(), state.children.ctx_need.size(), REAL(r_ctx_need));
    ret.push_back(r_hc1_hiv_pop, "hc1_hiv_pop");
    ret.push_back(r_hc2_hiv_pop, "hc2_hiv_pop");
    ret.push_back(r_hc1_art_pop, "hc1_art_pop");
    ret.push_back(r_hc2_art_pop, "hc2_art_pop");
    ret.push_back(r_hc1_noart_aids_deaths, "hc1_noart_aids_deaths");
    ret.push_back(r_hc2_noart_aids_deaths, "hc2_noart_aids_deaths");
    ret.push_back(r_hc1_art_aids_deaths, "hc1_art_aids_deaths");
    ret.push_back(r_hc2_art_aids_deaths, "hc2_art_aids_deaths");
    ret.push_back(r_hc_art_num, "hc_art_num");
    ret.push_back(r_hiv_births, "hiv_births");
    ret.push_back(r_hc_art_total, "hc_art_total");
    ret.push_back(r_hc_art_init, "hc_art_init");
    ret.push_back(r_hc_art_need_init, "hc_art_need_init");
    ret.push_back(r_ctx_need, "ctx_need");
  }

  return ret;
}

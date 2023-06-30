// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <RcppEigen.h>
#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// run_base_model
Rcpp::List run_base_model(const Rcpp::List data, const Rcpp::List projection_parameters, SEXP sim_years, SEXP hiv_steps_per_year, std::string hiv_age_stratification, const int n_simulations);
RcppExport SEXP _frogger_run_base_model(SEXP dataSEXP, SEXP projection_parametersSEXP, SEXP sim_yearsSEXP, SEXP hiv_steps_per_yearSEXP, SEXP hiv_age_stratificationSEXP, SEXP n_simulationsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::List >::type data(dataSEXP);
    Rcpp::traits::input_parameter< const Rcpp::List >::type projection_parameters(projection_parametersSEXP);
    Rcpp::traits::input_parameter< SEXP >::type sim_years(sim_yearsSEXP);
    Rcpp::traits::input_parameter< SEXP >::type hiv_steps_per_year(hiv_steps_per_yearSEXP);
    Rcpp::traits::input_parameter< std::string >::type hiv_age_stratification(hiv_age_stratificationSEXP);
    Rcpp::traits::input_parameter< const int >::type n_simulations(n_simulationsSEXP);
    rcpp_result_gen = Rcpp::wrap(run_base_model(data, projection_parameters, sim_years, hiv_steps_per_year, hiv_age_stratification, n_simulations));
    return rcpp_result_gen;
END_RCPP
}
// serialize_vector
Rcpp::List serialize_vector(const Rcpp::List data, const std::string path1, const std::string path2);
RcppExport SEXP _frogger_serialize_vector(SEXP dataSEXP, SEXP path1SEXP, SEXP path2SEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::List >::type data(dataSEXP);
    Rcpp::traits::input_parameter< const std::string >::type path1(path1SEXP);
    Rcpp::traits::input_parameter< const std::string >::type path2(path2SEXP);
    rcpp_result_gen = Rcpp::wrap(serialize_vector(data, path1, path2));
    return rcpp_result_gen;
END_RCPP
}
// deserialize_vector
Rcpp::List deserialize_vector(const std::string path1, const std::string path2);
RcppExport SEXP _frogger_deserialize_vector(SEXP path1SEXP, SEXP path2SEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string >::type path1(path1SEXP);
    Rcpp::traits::input_parameter< const std::string >::type path2(path2SEXP);
    rcpp_result_gen = Rcpp::wrap(deserialize_vector(path1, path2));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_frogger_run_base_model", (DL_FUNC) &_frogger_run_base_model, 6},
    {"_frogger_serialize_vector", (DL_FUNC) &_frogger_serialize_vector, 3},
    {"_frogger_deserialize_vector", (DL_FUNC) &_frogger_deserialize_vector, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_frogger(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}

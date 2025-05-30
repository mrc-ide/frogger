% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/run_model.R
\name{run_model_single_year}
\alias{run_model_single_year}
\title{Run leapfrog model fit for a single year}
\usage{
run_model_single_year(
  parameters,
  configuration,
  initial_state,
  start_from_year
)
}
\arguments{
\item{parameters}{Projection parameters}

\item{configuration}{The model configuration to run, see
\code{\link[=list_model_configurations]{list_model_configurations()}} for available configurations}

\item{initial_state}{The model will run from this initial state}

\item{start_from_year}{Start the model simulation from this year}
}
\value{
List of model outputs without the last time dimension.
This is different from \code{\link[=run_model_from_state]{run_model_from_state()}} and \code{\link[=run_model]{run_model()}}
that do include the last time dimension. Since only the next time
step is returned, dropping the time dimensions makes it easier to
feed the returned list into the next single year model run. In
contrast to \code{\link[=run_model_from_state]{run_model_from_state()}} the \code{p_total_pop} output will
have dimensions 81 x 2 not 81 x 2 x 61.
}
\description{
Run leapfrog model fit for a single year
}

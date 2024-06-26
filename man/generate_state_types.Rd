% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/generate_cpp.R
\name{generate_state_types}
\alias{generate_state_types}
\title{Generate C++ for state types}
\usage{
generate_state_types(
  dest,
  output_csv = frogger_file("cpp_generation/model_output.csv")
)
}
\arguments{
\item{dest}{The destination to write generated code to.}

\item{output_csv}{Path to csv file with output specification in it}
}
\value{
Nothing, called to generate code in src dir
}
\description{
Generate C++ for state types
}
\keyword{internal}

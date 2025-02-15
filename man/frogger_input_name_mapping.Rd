% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/generate_cpp.R
\name{frogger_input_name_mapping}
\alias{frogger_input_name_mapping}
\title{Get mapping of R names to C++ names for input data}
\usage{
frogger_input_name_mapping(
  input_csv = frogger_file("cpp_generation/model_input.csv")
)
}
\arguments{
\item{input_csv}{Path to input csv which is source of truth for these names}
}
\value{
A list with r_name, cpp_name, and C++ struct/namespace this
belongs to
}
\description{
Get mapping of R names to C++ names for input data
}

#!/usr/bin/env Rscript

usage <- "Run leapfrog model and save output to specified dir
Usage:
  run_model <output-dir>

Arguments:
  <output-dir>  Path to save output to.

Options:
  -h --help                  Show this screen.
"

dat <- docopt::docopt(usage)
names(dat) <- gsub("-", "_", names(dat), fixed = TRUE)

if (!dir.exists(dat$output_dir)) {
  dir.create(dat$output_dir, recursive = TRUE)
}

parameters <- frogger::read_parameters(testthat::test_path("testdata/adult_parms.h5"))
out <- frogger::run_model(parameters)

frogger:::save_hdf5_file(out, file.path(dat$output_dir, "r-output.h5"))

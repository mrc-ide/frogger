skip_for_compilation <- function() {
  testthat::skip_on_cran()
}

copy_directory <- function(src, as) {
  files <- dir(src, all.files = TRUE, no.. = TRUE, full.names = TRUE)
  dir.create(as, FALSE, TRUE)
  ok <- file.copy(files, as, recursive = TRUE)
  if (!all(ok)) {
    stop("Error copying files")
  }
}

expect_contains <- function(expected, full_text) {
  expect_true(any(grepl(expected, full_text, fixed = TRUE)))
}

expect_string <- function(x, err_msg = deparse(substitute(x))) {
  expect_true(!is.na(x) && nzchar(x),
              sprintf("Expected non empty string: %s", err_msg))
}

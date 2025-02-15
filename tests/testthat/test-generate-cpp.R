testthat::skip("Skipping temporarily whilst updating structure")

test_that("can generate output parsing", {
  t <- tempfile()
  generate_output_interface(t)

  result <- readLines(t)
  expect_contains("// Generated by frogger: do not edit by hand", result)
  expect_contains(
    paste0(
      "Rcpp::NumericVector r_p_total_pop",
      "(base.pAG * base.NS * output_years);"
    ),
    result
  )
  expect_contains(
    paste0(
      "r_p_total_pop.attr(\"dim\") = Rcpp::NumericVector::",
      "create(base.pAG, base.NS, output_years);"
    ),
    result
  )
  expect_contains(
    paste0(
      "std::copy_n(state.base.p_total_pop.data(), ",
      "state.base.p_total_pop.size(), REAL(r_p_total_pop));"
    ),
    result
  )
  expect_contains(
    "names[0] = \"p_total_pop\";",
    result
  )
  expect_contains(
    "ret[0] = r_p_total_pop;",
    result
  )
})

test_that("can generate input parsing", {
  t <- tempfile()
  generate_input_interface(t)

  result <- readLines(t)
  expect_contains("// Generated by frogger: do not edit by hand", result)
  expect_contains(
    paste0(
      "const leapfrog::TensorMap2<real_type> base_pop = parse_data<real_type>",
      "(data, \"basepop\", base.pAG, base.NS);"
    ),
    result
  )
  expect_contains(
    paste0(
      "const leapfrog::Tensor1<int> idx_hm_elig = convert_0_based<1>(",
      "parse_data<int>(data, \"artcd4elig_idx\", proj_years));"
    ),
    result
  )
  expect_contains(
    "leapfrog::Tensor1<real_type> h_art_stage_dur(base.hTS - 1);",
    result
  )
  expect_contains(
    "h_art_stage_dur.setConstant(0.5);",
    result
  )
  expect_contains(
    "const leapfrog::Demography<real_type> demography_params = {",
    result
  )
  expect_contains(
    "const leapfrog::Children<real_type> children_params = {",
    result
  )
})

# Uncomment once code generation working for python
# test_that("generated files are up to date", {
#   generation_funcs <- list(
#     "model_input.hpp" = generate_input_interface,
#     "model_output.hpp" = generate_output_interface,
#     "parameter_types.hpp" = generate_parameter_types,
#     "state_types.hpp" = generate_state_types,
#     "state_saver_types.hpp" = generate_state_saver_types
#   )
#   generated_files <- list.files(frogger_file("include/generated"))
#   expect_setequal(names(generation_funcs), generated_files)
#
#   for (file_name in names(generation_funcs)) {
#     target_generated_file <- frogger_file("include/generated", file_name)
#     t <- tempfile()
#     generation_funcs[[file_name]](t)
#     expect_identical(
#       readLines(t), readLines(target_generated_file),
#       info = paste0("Your interface is out of date, regenerate by running ",
#                     "./scripts/generate")
#     )
#   }
# })

test_that("validate_and_parse_dims", {
  row <- data.frame(
    r_name = "basepop", cpp_name = "base_pop",
    type = "real_type", dims = "2", dim1 = "pAG",
    dim2 = "NS", dim3 = "", dim4 = NA_character_
  )
  expect_equal(
    validate_and_parse_dims(row, "inputs.csv", 4),
    c("pAG", "NS")
  )


  row$dim3 <- "proj_years"
  expect_error(
    validate_and_parse_dims(row, "inputs.csv", 4),
    "Expected 2 dimensions for row 4 but got 3, check 'inputs.csv'."
  )

  row$dim2 <- ""
  expect_error(
    validate_and_parse_dims(row, "inputs.csv", 4),
    paste0(
      "'dim3' set in row 4 but not 'dim2', dimensions must ",
      "be set in order. Check 'inputs.csv'."
    )
  )

  row$dim3 <- ""
  row$dim4 <- "proj_years"
  expect_error(
    validate_and_parse_dims(row, "inputs.csv", 4),
    paste0(
      "'dim4' set in row 4 but not 'dim2', 'dim3', dimensions must ",
      "be set in order. Check 'inputs.csv'."
    )
  )
})

test_that("csv structure can be validated", {
  colnames <- c("r_name", "cpp_name", "dims", "dim1", "dim2")
  expect_silent(validate_dimensions_columns(colnames, "inputs.csv"))

  colnames <- c(colnames, "other")
  expect_error(
    validate_dimensions_columns(colnames, "inputs.csv"),
    paste(
      "All columns after 'dims' must be a dimension column, got 'other'.",
      "Check 'inputs.csv'."
    )
  )
})

test_that("can generate length 1 inputs", {
  input <- utils::read.csv(frogger_file("cpp_generation/model_input.csv"))
  length_1_input <- data.frame(
    r_name = "len1", cpp_name = "Demography.len_1",
    type = "real_type", input_when = "",
    value = NA_character_,
    convert_0_based = FALSE, dims = 1, dim1 = 1,
    dim2 = "", dim3 = "", dim4 = ""
  )
  input <- rbind.data.frame(input, length_1_input)
  t_input <- tempfile()
  write.csv(input, t_input)
  t_dest <- tempfile()

  generate_input_interface(t_dest, t_input)
  result <- readLines(t_dest)
  expect_contains(
    "const real_type len_1 = Rcpp::as<real_type>(data[\"len1\"])",
    result
  )
})

test_that("output optional data generated in conditional block", {
  t <- tempfile()
  generate_output_interface(t)

  result <- readLines(t)

  opening_condition <- which(grepl(
    "if constexpr \\([A-Za-z:_]+\\) \\{",
    result
  ))[1]
  closing <- which(grepl("^\\s+\\}$", result))

  closing_bracket <- closing[closing > opening_condition][1]

  optional_data <- which(grepl(
    paste0(
      "Rcpp::NumericVector r_hc1_hiv_pop\\(children.hc1DS ",
      "\\* children.hcTT \\* children.hc1AG \\* base.NS \\* output_years\\);"
    ),
    result
  ))
  expect_true(optional_data > opening_condition)
  expect_true(optional_data < closing_bracket)
})

test_that("input optional data generated in conditional block", {
  t <- tempfile()
  generate_input_interface(t)

  result <- readLines(t)

  opening_condition <- which(grepl(
    "if constexpr \\([A-Za-z:_]+\\) \\{",
    result
  ))[1]
  else_statement <- which(grepl("^\\s+\\} else \\{$", result))[1]
  closing <- which(grepl("^\\s+\\}$", result))

  closing_bracket <- closing[closing > opening_condition][1]

  optional_data <- which(grepl(
    paste0(
      "const leapfrog::TensorMap1<real_type> hc_nosocomial =",
      " parse_data<real_type>\\(data, \"paed_incid_input\", proj_years\\);"
    ),
    result
  ))
  expect_true(optional_data > opening_condition)
  expect_true(optional_data < else_statement)
})

test_that("camel_to_snake works", {
  expect_equal(camel_to_snake(""), "")
  expect_equal(camel_to_snake("Art"), "art")
  expect_equal(camel_to_snake("NaturalHistory"), "natural_history")
  expect_equal(camel_to_snake("MyLongString"), "my_long_string")
  expect_equal(camel_to_snake("ART"), "a_r_t")
  expect_equal(camel_to_snake("MYString"), "m_y_string")
  expect_equal(camel_to_snake("myString"), "my_string")
  expect_equal(camel_to_snake("childART"), "child_a_r_t")
})

test_that("can generate parameter types", {
  t <- tempfile()
  generate_parameter_types(t)

  result <- readLines(t)
  expect_contains("// Generated by frogger: do not edit by hand", result)
  expect_contains("struct Demography {", result)
  expect_contains("TensorMap2<real_type> base_pop;", result)
  expect_contains("real_type ctx_effect;", result)
  expect_contains("TensorMap1<int> hc_art_isperc;", result)
  expect_contains("int scale_cd4_mortality;", result)
  expect_contains("Tensor1<int> idx_hm_elig;", result)
  expect_contains("Tensor1<real_type> h_art_stage_dur;", result)
})

test_that("error raised if cpp_name malformed", {
  input <- data.frame(
    r_name = "r_name", cpp_name = "name",
    type = "real_type", input_when = "",
    value = NA_character_,
    convert_0_based = FALSE, dims = 1, dim1 = 1,
    dim2 = "", dim3 = "", dim4 = ""
  )
  t_input <- tempfile()
  write.csv(input, t_input)
  t_dest <- tempfile()

  expect_error(
    generate_input_interface(t_dest, t_input),
    paste(
      "Each value in column 'cpp_name' must have a value of",
      "format 'x.y' where x is the struct name and y is the",
      "name of the variable. Got 'name'."
    )
  )
})

test_that("error raised if output csv doesn't have output_years as last dim", {
  output <- data.frame(
    r_name = "r_name", cpp_name = "name",
    r_type = "REAL", output_when = "",
    dims = 2, dim1 = "thing",
    dim2 = "this", dim3 = "", dim4 = ""
  )
  t_output <- tempfile()
  write.csv(output, t_output)
  t_dest <- tempfile()

  expect_error(
    generate_state_types(t_dest, t_output),
    paste(
      "Last dimension of model output must be 'output_years'.",
      "Got 'this' for output 'r_name'."
    )
  )
})

test_that("can generate state types", {
  t <- tempfile()
  generate_state_types(t)

  result <- readLines(t)
  expect_contains("// Generated by frogger: do not edit by hand", result)
  expect_contains("struct BaseModelState {", result)
  expect_contains(paste0(
    "TensorFixedSize<real_type, Sizes<pAG<ModelVariant>, ",
    "NS<ModelVariant>>> p_total_pop"
  ), result)
  expect_contains("real_type births;", result)
  expect_contains("p_total_pop.setZero();", result)
  expect_contains("births = 0;", result)
  expect_contains("struct ChildModelState<ChildModel, real_type>", result)
  expect_contains(paste0(
    "TensorFixedSize<real_type, Sizes<hc1DS<ChildModel>, hcTT<ChildModel>, ",
    "hc1AG<ChildModel>, NS<ChildModel>>> hc1_hiv_pop;"
  ), result)
  expect_contains("hc1_hiv_pop.setZero();", result)
})

test_that("can generate state saver types", {
  t <- tempfile()
  generate_state_saver_types(t)

  result <- readLines(t)
  expect_contains("// Generated by frogger: do not edit by hand", result)
  expect_contains("struct BaseModelOutputState {", result)
  expect_contains("Tensor3<real_type> p_total_pop", result)
  expect_contains("p_total_pop(", result)
  expect_contains("StateSpace<ModelVariant>().base.pAG,", result)
  expect_contains("p_total_pop.setZero();", result)
  expect_contains("struct ChildModelOutputState {", result)
  expect_contains("struct ChildModelOutputState<ChildModel, real_type>", result)
  expect_contains("hc1_hiv_pop(", result)
  expect_contains("StateSpace<ChildModel>().children.hc1DS,", result)
  expect_contains("hc1_hiv_pop.setZero();", result)

  expect_contains("struct BaseModelStateSaver {", result)
  expect_contains(paste0("void save_state(BaseModelOutputState<ModelVariant, ",
                         "real_type> &output_state,"), result)
  expect_contains("struct ChildModelStateSaver {", result)
  expect_contains("struct ChildModelStateSaver<ChildModel, real_type> {", result)
})

test_that("can get mapping of R name to C++ name for input data", {
  mapping <- frogger_input_name_mapping()
  for (map in mapping) {
    expect_setequal(names(map), c("r_name", "cpp_name", "struct"))
    expect_string(map$r_name)
    expect_string(map$cpp_name)
    expect_string(map$struct)
  }
})

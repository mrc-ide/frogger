validate_and_parse_input <- function(input, filename, row_num) {
  row_text <- paste("row: ", row_num)
  assert_only_one_set(input, c("r_name", "value"),
                      name = row_text)
  assert_set(input$cpp_name, paste(row_text, "and col: cpp_name"))
  assert_enum(input$type, c("real_type", "int"),
              name = paste(row_text, "and col: type"))
  assert_enum(input$convert_base, c("FALSE", "TRUE", ""),
              name = paste(row_text, "and col: convert_base"))
  input$convert_base <- identical(input$convert_base, "TRUE")
  assert_set(input$dims)
  input$parsed_dims <- validate_and_parse_dims(input, filename, row_num)
  input
}

validate_and_parse_output <- function(output, filename, row_num) {
  row_text <- paste("row: ", row_num)
  assert_set(output$r_name)
  assert_set(output$cpp_name)
  assert_enum(output$r_type, c("REAL", "INTEGER"),
              name = paste(row_text, "and col: r_type"))
  assert_set(output$dims)
  output$parsed_dims <- validate_and_parse_dims(output, filename, row_num)
  output
}

validate_dimensions_columns <- function(columns, filename) {
  dims_col <- which(columns == "dims")
  ## All columns after "dims" must be named dim1, dim2, .., dimi, etc.
  dimension_data <- columns[seq(dims_col + 1, length(columns))]
  invalid_columns <- !grepl("dim\\d", dimension_data)
  if (any(invalid_columns)) {
    invalid_column_names <- dimension_data[invalid_columns]
    stop(sprintf(paste("All columns after 'dims' must be a dimension",
                       "column, got %s. Check '%s'."),
                 format_vector(invalid_column_names), filename))
  }
  invisible(TRUE)
}

validate_and_parse_dims <- function(data, filename, row_num) {
  dims_col <- which(names(data) == "dims")
  dims <- data$dims

  ## We've validated already dimension columns come after the dims column
  ## e.g. csv has col1, col2, col3, ..., dims, dim1, dim2, dim3, ..., dimn
  ## dims are valid if they have same number set as specified in `dim` column
  set_dims <- vlapply(data[seq(dims_col + 1, length(data))], is_set)
  if (!identical(as.character(sum(set_dims)), dims)) {
    stop(sprintf("Expected %s dimensions for row %s but got %s, check '%s'.",
                 dims, row_num, sum(set_dims), filename))
  }
  ## also invalid if they are not set in order i.e. if dim3 is set,
  ## dim2 and dim1 must be we don't allow dim1 to be set, dim2 empty and
  ## dim3 set. We could allow this but I expect if we get this in the csv
  ## it is indicative of something being specified incorrectly so I want
  ## to error
  last_set <- max(which(set_dims))
  unset <- names(set_dims)[!set_dims[seq(1, last_set)]]
  if (length(unset) > 0) {
    stop(sprintf(paste("'%s' set in row %s but not %s, dimensions",
                       "must be set in order. Check '%s'."),
                 names(set_dims)[last_set], row_num,
                 format_vector(unset), filename))
  }
  as.character(data[seq(dims_col + 1, dims_col + last_set)])
}

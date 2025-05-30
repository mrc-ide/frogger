PACKAGE := $(shell grep '^Package:' DESCRIPTION | sed -E 's/^Package:[[:space:]]+//')
RSCRIPT = Rscript --no-init-file

all:
	${RSCRIPT} -e 'pkgbuild::compile_dll()'

test:
	${RSCRIPT} -e 'devtools::test()'

roxygen:
	@mkdir -p man
	${RSCRIPT} -e "devtools::document()"

install:
	R CMD INSTALL .

build:
	R CMD build .

README.md: README.Rmd
	Rscript -e 'devtools::load_all(); rmarkdown::render("README.Rmd")'
	sed -i.bak 's/[[:space:]]*$$//' README.md
	rm -f $@.bak

test_leaks:
	R -d 'valgrind --leak-check=full --error-exitcode=1' -e 'testthat::test_local(".")'

check:
	_R_CHECK_CRAN_INCOMING_=FALSE make check_all

check_all:
	${RSCRIPT} -e "rcmdcheck::rcmdcheck(args = c('--as-cran', '--no-manual'))"

clean:
	rm -f src/*.o src/*.so src/*.gcda src/*.gcno src/*.gcov

.PHONY: clean all test document install

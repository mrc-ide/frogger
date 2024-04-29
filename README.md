# frogger

<!-- badges: start -->
[![Project Status: Concept – Minimal or no implementation has been done yet, or the repository is only intended to be a limited example, demo, or proof-of-concept.](https://www.repostatus.org/badges/latest/concept.svg)](https://www.repostatus.org/#concept)
[![R build status](https://github.com/mrc-ide/frogger/workflows/R-CMD-check/badge.svg)](https://github.com/mrc-ide/frogger/actions)
[![codecov.io](https://codecov.io/github/mrc-ide/frogger/coverage.svg?branch=main)](https://codecov.io/github/mrc-ide/frogger?branch=main)
[![CodeFactor](https://www.codefactor.io/repository/github/mrc-ide/frogger/badge)](https://www.codefactor.io/repository/github/mrc-ide/frogger)
<!-- badges: end -->

## Installation

To install `frogger`:

```r
remotes::install_github("mrc-ide/frogger", upgrade = FALSE)
```

## TODO

* Improve variable names!
* Restructuring the model code to identify more common code
    * There are examples like general demographic projection and hiv population demographic projection which are running
      similar processes like ageing, non HIV mortality, migration. We should be able to write a function for e.g. ageing
      which we can run on each of our population matrices. Even for the HIV and ART stratified we can add overloaded
      function to work with higher dimension data
* Add a test that checks that no `double`s are used in `inst/include` dir. We should be using templated `real_type` for
  TMB
* Add a broad level overview of the algorithm - is there a diagram available?
* Convert string flags to the model to enums if we need to switch on them in several places. This should make it easier
  to reason about in C++ world and isolate the string checking to a single place
* Update `gender` terminology to sex
* Previously `hiv_negative_pop` was fixed size by having dimensions specified by template, how much does this speed up
  the code? Is there a better way to do this?
* Tidy up confusing looping see https://github.com/mrc-ide/frogger/pull/7#discussion_r1217847753
* Add R casting helpers which return better errors than Rcpp
  see https://github.com/mrc-ide/frogger/pull/7#discussion_r1217884684
* Add a helper to do 0 to base 1 conversion and check upper bounds
  see https://github.com/mrc-ide/frogger/pull/7#discussion_r1217888684
* Review what we pass as parameters - can some of these be computed in the struct ctor?
  e.g. https://github.com/mrc-ide/frogger/pull/7#discussion_r1217890466
* Refactor `OutputState` to take a struct of state-space dimensions instead of unpacking the subset of parameters we
  need. See https://github.com/mrc-ide/frogger/pull/12#discussion_r1245170775

## Leapfrog to Frogger glossary

Frogger naming rules

* `p_` prefix indicates this is age stratified by total single year i.e. same age stratification as total population
* `h_` prefix indicates this is age stratified by same age stratification as HIV adult population, this might be single
  year from
  15+ or could be coarse age groups
* `hc_` prefix indicates this is age stratified by the HIV child population
* `hc1_` prefix indicates this is age stratified by child population age 0 - 4
* `hc2_` prefix indicates this is age stratified by the child population age 5 - 14
* `idx_` prefix means this is an index, e.g. `idx_hm_elig` is the index of the CD4 count at which people are eligible
  for ART. Note this is not used for prefixing the loop variables
* Uppercase means this is a dimension of the state space e.g. `NS` or `pAG`
* `ts_` prefix means this is a time step
* `hts_` prefix means this is an HIV time step (i.e. a time step of the inner HIV loop)

These prefixes can be merged e.g.

* `p_idx_hiv_first_adult` - Is the index of the first age group within singe-year ages to be considered an adult

### State space

In `StateSpace` struct

| Leapfrog | Frogger  | Details                                                        |
|----------|----------|----------------------------------------------------------------|
| NG       | NS       | Number of sexes                                                |
| pAG      | pAG      | Number of age groups in population, 81 for 0 to 80+            |
| hAG      | hAG      | Number of age groups in coarse stratified adult HIV population |
| hDS      | hDS      | Number of disease stages in adult HIV population               |
| hTS      | hTS      | Number of treatment stages in adult HIV population             |
| hAG_SPAN | hAG_span | Array of HIV age group sizes                                   |
|                                                                                      |
| hDS_paed | hc1DS    | Number of disease stages in the 0-4 population                 |
|                                                                                      |
| hDS_adol | hc2DS    | Number of disease stages in the 5-14 population                | 
|                                                                                      |
| hTM      | hcTT     | Number of transmission types for the paediatric population     |
|                                                                                      |
| NA       | hc1AG    | Number of age groups in the 0-4 population                     |
|                                                                                      |
| NA       | hc2AG    | Number of age groups in the 5-14 population                    |
|                                                                                      |
| hBF       | hBF      | Number of 2 month breastfeeding durations                     |

#### Loop variable convention

* `NS` is `s`
* `pAG` is `a`
* `hAG` is `ha`
* `hDS`, `hc1DS`, and `hc2DS` are `hd`
* `hcTT` is `cat`
* `hTS` is `ht`
* `hBF` is `bf`


### Key indices in state space or other options controlling fit

In `Options` struct. Some of these come from data, some are static, some are an input option

| Leapfrog           | Frogger                | Details                                                |
|--------------------|------------------------|--------------------------------------------------------|
| hiv_steps_per_year | hts_per_year           | Number of HIV model time steps per year                |
| pIDX_FERT          | p_idx_fertility_first  | First age group index eligible for fertility           |
| pAG_FERT           | p_fertility_age_groups | Number of ages eligible for fertility                  |
| pIDX_HIVADULT      | p_idx_hiv_first_adult  | Index of the first age group to be considered an adult |
| t_ART_start        | ts_art_start           | Time step to start modelling ART                       |
| NA                 | run_child_model        | Whether the paediatric model should be run             |


### Input data

In 5 structs as part of the `Parameters`, named `Demography`, `Incidence`, `NaturalHistory`, `Art` and `Children`

| Leapfrog           | Frogger                                  | Details                                                                                                                                                               |
|--------------------|------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| basepop            | demography.base_pop                      | Population data by age group and sex                                                                                                                                  |
| sx                 | demography.survival_probability          | Probability of surviving from age x to x+1 i.e. from 0 to 1, 1 to 2, ..., 79 to 80+ and 80+ to 80+                                                                    |
| netmigr            | demography.net_migration                 | Net migration by age group, sex and year                                                                                                                              |
| asfr               | demography.age_specific_fertility_rate   | Rate of live births in a year and the total female population of childbearing age                                                                                     |
| births_sex_prop    | demography.births_sex_prop               | Proportion of male and female births each year                                                                                                                        |
| incidinput         | incidence.total_rate                     | Incidence rate per year                                                                                                                                               |
| incrr_sex          | incidence.sex_rate_ratio                 | HIV incidence rate ratio for female : male age 15-49 years                                                                                                            |
| incrr_age          | incidence.age_rate_ratio                 | HIV incidence rate ratio by age (for each sex)                                                                                                                        |
| cd4_initdist       | natural_history.cd4_initial_distribution | Distribution of infections by CD4 category upon infection                                                                                                             |
| cd4_prog           | natural_history.cd4_progression          | Probability of progressing from 1 CD4 stage to the next by age and sex                                                                                                |
| cd4_mort           | natural_history.cd4_mortality            | Probability of mortality by CD4 stage, age and sex                                                                                                                    |
| scale_cd4_mort     | natural_history.scale_cd4_mortality      | If 1 then scale HIV related mortality (i.e. cd4_mortality) as a proportion of number of people with HIV and over the number with HIV and on ART at this disease stage |
| art_mort           | art.mortality_rate                       | Probability of mortality by treatment stage, CD4 stage, age and sex                                                                                                   |
| artmx_timerr       | art.mortaility_time_rate_ratio           | ART mortality rate ratio by year for <12 months and >12 months on ART                                                                                                 |
| art15plus_num      | art.adults_on_art                        | Time series of # or % of adult PLHIV on ART by sex                                                                                                                    |
| art15plus_isperc   | art.adults_on_art_is_percent             | Time series, TRUE if art15plus_num is a %, FALSE if it is a #                                                                                                         |
| artcd4elig_idx     | art.idx_hm_elig                          | The index of the CD4 count at which people are eligible for ART by time step                                                                                          |
| art_alloc_mxweight | art.initiation_mortality_weight          | Weighting for extent that expected mortality guides ART uptake                                                                                                        |
| paed_incid_input   | chidlren.hc_nosocomial                   | Nosocomial infections occuring in 0-4 year olds                                                                                                                       |
| paed_cd4_dist      | children.hc1_cd4_dist                    | Distribution of infections by CD4 category by infection type                                                                                                          |
| paed_cd4_transition| children.hc_cd4_transition               | CD4 mapping between 0-4 year olds and 5-14 year olds                                                                                                                  |
| paed_cd4_mort      | children.hc1_cd4_mort                    | Probability of mortality by CD4 stage, age, and sex for 0-4 year olds                                                                                                 |
| adol_cd4_mort      | children.hc2_cd4_mort                    | Probability of mortality by CD4 stage, age, and sex for 5-14 year olds                                                                                                |                               
| paed_cd4_prog      | children.hc1_cd4_prog                    | Probability of progressing to the subsequent CD4 stage for 0-4 year olds                                                                                              |
| adol_cd4_prog      | children.hc2_cd4_prog                    | Probability of progressing to the subsequent CD4 stage for 5-14 year olds                                                                                             |                                               
| ctx_effect         | children.ctx_effect                      | Effectiveness of cotrimoxazole at averting mortality                                                                                                                  |
| ctx_val            | children.ctx_val                         | Number of children receiving cotrimoxazole                                                                                                                            |
| paed_art_elig_age  | children.hc_art_elig_age                 | Age at which children are eligible for ART by year                                                                                                                    |
| paed_art_elig_cd4  | children.hc_art_elig_cd4                 | CD4 at which children are eligible for ART by year                                                                                                                    |
| mort_art_rr        | children.hc_art_mort_rr                  | Mortality rate ratios by age and treatment duration                                                                                                                   |
| paed_art_mort      | children.hc1_art_mort                    | Mortality rate for children 0-4y by CD4 and treatment duration                                                                                                        |
| adol_art_mort      | children.hc2_art_mort                    | Mortality rate for children 5-14y by CD4 and treatment duration                                                                                                       |
| artpaeds_isperc    | children.hc_art_isperc                   | Boolean vector indicating whether input is a percent                                                                                                                  |
| paed_art_val       | children.hc_art_val                      | Percent or number of children on ART each year                                                                                                                        |
| init_art_dist      | children.hc_art_init_dist                | ART initiation by age                                                                                                                                                 |
| adult_cd4_dist     | children.adult_cd4_dist                  | Transition matrix converting 5-14y CD4 categories to adult CD4 categories                                                                                             |
| fert_mult_by_age   | children.fert_mult_by_age                | Fertility ratio for WLHIV by age                                                                                                                                      |
| fert_mult_offart   | children.fert_mult_offart                | Fertility ratio for WLHIV not on treatment by CD4                                                                                                                     |
| fert_mult_onart    | children.fert_mult_onart                 | Fertility ratio for WLHIV on treatment by age                                                                                                                         |
| tfr                | children.total_fertility_rate            | Total fertility rate                                                                                                                                                  |
| laf                | children.local_adj_factor                | Local fertility adjustment to match progromattic data                                                                                                                 |
| pmtct              | children.PMTCT                           | Number of percent of women receiving ARVs for PMTCT                                                                                                                   |
| mtct               | children.vertical_transmission_rate      | Vertical transmsision rate for pregnant and breastfeeding women not receiving ARV by CD4 category                                                                     |
| pmtct_mtct         | children.PMTCT_transmission_rate         | Vertical transmission rate for pregnant and breastfeeding women by PMTCT regimen                                                                                      |
| pmtct_dropout      | children.PMTCT_dropout                   | Dropout rates for women receiving ARV for PMTCT during pregnancy and breastfeeding                                                                                    |
| pmtct_input_is_perc| children.PMTCT_input_is_percent          | Boolean vector indicating whether PMTCT values were input as percent or number                                                                                        |
| bf_duration_art    | children.breastfeeding_duration_art      | Percent of women on ART still breastfeeding at two month invervals                                                                                                    |
| bf_duration_no_art | children.breastfeeding_duration_no_art   | Percent of women not on ART still breastfeeding at two month intervals                                                                                                |
| mat_hiv_births     | children.mat_hiv_births                  | Number of births to WLHIV                                                                                                                                             |
| mat_prev_input     | children.mat_prev_input                  | Boolean vector indicating if input mat_hiv_births should be used for vertical transmission rather than results from adult model                                       |
| prop_lt200         | children.prop_lt200                      | Proportion of WLHIV with CD4 less than 200 by year                                                                                                                    |
| prop_gte350        | children.prop_gte350                     | Proportion of WLHIV with CD4 greater than 350 by year                                                                                                                 |
| incrate            | children.incrate                         | Incidence rate among women 15-49y by year                                                                                                                             |

### Outputs

Discussion: naming conventions

* Distinguish events vs. counts
* Distinguish stratification of array (single-year population pXX vs. HIV population hXX

| Leapfrog         | Frogger                    | Details                                                                                    |
|------------------|----------------------------|--------------------------------------------------------------------------------------------|
| totpop1          | p_total_pop                | Projected total population                                                                 |
| hivpop1          | p_hiv_pop                  | Projected HIV population                                                                   |
| infections       | p_infections               | Projected number of new HIV infections by sex and age                                      |
| hivstrat_adult   | h_hiv_adult                | Projected PLHIV not on ART by age, sex and CD4                                             |
| artstrat_adult   | h_art_adult                | Projected PLHIV on ART by age, sex, CD4 and treatment stage                                |
| births           | births                     | Projected number of births                                                                 |
| natdeaths        | p_total_pop_natural_deaths | Projected number of natural deaths                                                         |
| natdeaths_hivpop | p_hiv_pop_natural_deaths   | Projected number of natural deaths within HIV population                                   |
| hivdeaths        | p_hiv_deaths               | Projected HIV-related deaths by sex and age                                                |
| aidsdeaths_noart | h_hiv_deaths_no_art        | Projected HIV-related deaths off ART by sex, age and CD4                                   |
| aidsdeaths_art   | h_hiv_deaths_art           | Projected HIV-related deaths on ART by sex, age, CD4 and treatment stage                   |
| artinit          | h_art_initiation           | Projected ART initiations by sex, age and CD4                                              |
| hivstrat_paeds   | hc1_hiv_pop                | Projected PLHIV 0-4y not on ART by age, sex, transmission type, and CD4                    |
| hivstrat_paeds   | hc2_hiv_pop                | Projected PLHIV 5-14y not on ART by age, sex, transmission type, and CD4                   |
| artstrat_paeds   | hc1_art_pop                | Projected PLHIV 0-4y on ART by age, sex, treatment stage and CD4                           |
| artstrat_paeds   | hc2_art_pop                | Projected PLHIV 5-14y on ART by age, sex, treatment stage and CD4                          |
|                  | hc1_noart_aids_deaths      | Projected deaths among PLHIV 0-4y not on ART by CD4, transmission type, age, sex, and year |
|                  | hc2_noart_aids_deaths      | Projected deaths among PLHIV 5-14y not on ART by CD4, transmission type, age, sex, and year|
|                  | hc1_art_aids_deaths        | Projected deaths among PLHIV 0-4y on ART by time on treatment, CD4, age, sex, and year     |
|                  | hc2_art_aids_deaths        | Projected deaths among PLHIV 5-14y on ART by time on treatment, CD4, age, sex, and year    |
|                  | hiv_births                 | Number of births to WLHIV by year                                                          |
|                  | hc_art_need_init           | Children who are eligible for ART by CD4, transmsission, age, sex, and year                |
|                  | hc_art_init                | Number of new ART initiates by five year age group.                                        |

### Internal

| Leapfrog                  | Frogger                         | Details                                                                   |
|---------------------------|---------------------------------|---------------------------------------------------------------------------|
| ART0MOS                   | ?                               |                                                                           |
| pIDX_INCIDPOP             | adult_incidence_first_age_group | Index of youngest age that is reflected in the adult incidence input      |
| pAG_INCIDPOP              | ?                               |                                                                           |
| hAG_15PLUS                | age_groups_hiv_15plus           |                                                                           |
| hIDX_15PLUS               | hIDX_15PLUS                     |                                                                           |
| h_art_stage_dur           | ?                               |                                                                           |
| everARTelig_idx           | everARTelig_idx                 |                                                                           |
| migrate_ag                | migration_rate                  | Rate people migrate into population by age and sex                        |
| sx_netmig                 | survival_netmig                 |                                                                           |
| births_sex                | births_sex                      |                                                                           |
| migrate_a0                | migration_rate_a0               |                                                                           |
| hiv_ag_prob               | hiv_age_up_prob                 | Probability of aging from one group to the next in HIV age stratification |
| hivpop_ha(ha, g)          | hiv_population_coarse_ages      |                                                                           |
| netmig_ag                 | hiv_net_migration               |                                                                           |
| deathsmig_ha              | deaths_migrate                  |                                                                           |
| deathmigrate_ha           | deaths_migrate_rate             |                                                                           |
| cd4elig_idx               | ?                               | Index of the maximum CD4 count category eligible for ART                  |
| anyelig_idx               | anyelig_idx                     |                                                                           |
| everARTelig_idx           | ?                               |                                                                           |
| infections_ts             | infections_ts                   | Infections occurring at a specific time step                              |
| hivn_ag                   | hiv_negative_pop                | HIV negative population for a given age and sex                           |
| Xhivn                     | hiv_neg_aggregate               |                                                                           |
| Xhivn_incagerr            | ?                               |                                                                           |
| incrate_i                 |                                 | Incidence rate input                                                      |
| incrate_g                 | rate_sex                        | Incidence rate by sex                                                     |
| hivdeaths_ha              | hiv_deaths_age_sex              |                                                                           |
| grad                      | grad                            | Movement between cd4 categories at a given time step                      |
| artpop_hahm               |                                 |                                                                           |
| cd4mx_scale               |                                 |                                                                           |
| infections_a              | infections_a                    |                                                                           |
| infections_ha             | infections_ha                   |                                                                           |
| gradART                   |                                 |                                                                           |
| artelig_hahm              |                                 | Number of PLHIV eligible for ART by age and disease stage                 |
| Xart_15plus               |                                 |                                                                           |
| Xartelig_15plus           |                                 |                                                                           |
| expect_mort_artelig15plus |                                 | Total mortality amongst ART eligible population                           |
| artnum_hts                |                                 |                                                                           |
| artcov_hts                |                                 |                                                                           |
| curr_coverage             |                                 |                                                                           |
| artinit_hts               | art_initiation                  | Total number by sex to initiate on ART by sex                             |
| artinit_hahm              |                                 | Number who initiate treatment by age and disease stage                    |
| hivpop_ha(ha)             | hivpop_ha                       | HIV population by coarse HIV age group stratification                     |
| hivqx_ha                  |                                 |                                                                           |
| hivdeaths_a               |                                 |                                                                           |
| age15_hivpop              | age15_hiv_pop                   | 15 year olds aging out of the paediatric population                       |
| deaths_paeds              | hc_posthivmort                  | PLHIV 0-14y stratified by age, sex, transmission type, and CD4 after      |
|                           |                                 | HIV related mortality                                                     |
| grad_paeds                | hc_grad                         | Difference in PLHIV 0-14 stratified by age, sex, transmission type,       |
|                           |                                 | and CD4 between the current and next timestep                             |
|                           | eligible                        | Children eligilbe for ART by CD4, age, and sex. Summed hc_art_need_init by|
|                           |                                 | transmission category.                                                    |
|                           | hc_death_rate                   | Death rate for children receiving treatment from the risk ratios and      |
|                           |                                 | combined death rates. By age.                                             |
|                           | hc_art_grad                     | Difference in CLHIV on ART in a given timestep by time on art, CD4, age,  |
|                           |                                 | and sex. Just used for on ART mortality.                                  |
|                           | hc_art_deaths                   | Number of deaths occuring among CLHIV on ART before initation. Needed to  |
|                           |                                 | calculate the number of children in need of tx. By five year age group.   |
|                           | unmet_need                      | Total number of children in need of ART initiation summed across all      |
|                           |                                 | categories by five year age group.                                        |
|                           | on_art                          | Children on ART by five year age group. Needed to calculate total need    |
|                           |                                 | for ART.                                                                  |
|                           | total_need                      | Children in need of ART in the current timestep by five year age group.   |
|                           |                                 | Sum of children on ART, those that need to initiate, and deaths in year t.|
|                           | total_art_last_year             | Number of CLHIV receiving ART in t-1.                                     |
|                           | total_art_this_year             | Number of CLHIV receiving ART in t.                                       |
|                           | total_art_last_year             | Number of CLHIV receiving ART in t-1.                                     |
|                           | retained                        | Proportion of CLHIV on ART after dropout.                                 |
|                           | hc_initByAge                    | Total number of children by five year age group that will initiate when   |
|                           |                                 | correcting for the probability of initiation from IeDEA data.             |
|                           | hc_adj                          | Proportion of all initiates that will intiatiate once corrected for prob  |
|                           |                                 | of initiation.                                                            |
|                           | hc_art_scalar                   |                                                                           |
|                           |                                 |                                                                           |
## License

MIT © Imperial College of Science, Technology and Medicine

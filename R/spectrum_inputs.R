read_start_year <- function(pjnz, use_ep5 = FALSE) {
  if(use_ep5) {
    dpfile <- grep(".ep5$", utils::unzip(pjnz, list = TRUE)$Name, value = TRUE)
  } else {
    dpfile <- grep(".DP$", utils::unzip(pjnz, list = TRUE)$Name, value = TRUE)
  }

  dp <- utils::read.csv(unz(pjnz, dpfile), as.is = TRUE)
  dpsub <- function(tag, rows, cols, tagcol =1 ) {
    dp[which(dp[, tagcol] == tag) + rows, cols]
  }
  as.integer(dpsub("<FirstYear MV2>",2,4))
}

read_sx <- function(pjnz, use_ep5=FALSE) {

  if(use_ep5) {
    dpfile <- grep(".ep5$", utils::unzip(pjnz, list=TRUE)$Name, value=TRUE)
  } else {
    dpfile <- grep(".DP$", utils::unzip(pjnz, list=TRUE)$Name, value=TRUE)
  }

  dp <- utils::read.csv(unz(pjnz, dpfile), as.is=TRUE)

  exists_dptag <- function(tag, tagcol=1) {
    tag %in% dp[,tagcol]
  }

  dpsub <- function(tag, rows, cols, tagcol=1) {
    dp[which(dp[,tagcol]==tag)+rows, cols]
  }

  ## projection parameters
  yr_start <- as.integer(dpsub("<FirstYear MV2>",2,4))
  yr_end <- as.integer(dpsub("<FinalYear MV2>",2,4))
  proj.years <- yr_start:yr_end
  timedat.idx <- 4+1:length(proj.years)-1

  ## mx
  Sx <- dpsub("<SurvRate MV2>", 3+c(0:81, 82+0:81), timedat.idx)
  Sx <- array(as.numeric(unlist(Sx)), c(82, 2, length(proj.years)))
  dimnames(Sx) <- list(age=c(0:80, "80+"), sex=c("male", "female"), year=proj.years)

  Sx
}

read_netmigr <- function(pjnz, use_ep5=FALSE, adjust_u5mig = TRUE, sx = NULL) {

  if(use_ep5) {
    dpfile <- grep(".ep5$", utils::unzip(pjnz, list=TRUE)$Name, value=TRUE)
  } else {
    dpfile <- grep(".DP$", utils::unzip(pjnz, list=TRUE)$Name, value=TRUE)
  }

  dp <- utils::read.csv(unz(pjnz, dpfile), as.is=TRUE)

  exists_dptag <- function(tag, tagcol=1) {
    tag %in% dp[,tagcol]
  }
  dpsub <- function(tag, rows, cols, tagcol=1) {
    dp[which(dp[,tagcol]==tag)+rows, cols]
  }

  ## projection parameters
  yr_start <- as.integer(dpsub("<FirstYear MV2>",2,4))
  yr_end <- as.integer(dpsub("<FinalYear MV2>",2,4))
  proj.years <- yr_start:yr_end
  timedat.idx <- 4+1:length(proj.years)-1

  ## netmig
  totnetmig <- sapply(dpsub("<MigrRate MV2>", c(4, 6), timedat.idx), as.numeric)

  netmigagedist <- sapply(dpsub("<MigrAgeDist MV2>", 2 + 1:34, timedat.idx), as.numeric)/100
  netmigagedist <- array(c(netmigagedist), c(17, 2, length(proj.years)))
  netmigr5 <- sweep(netmigagedist, 2:3, totnetmig, "*")

  netmigr <- array(dim = c(81, 2, length(proj.years)),
                   dimnames = list(age = 0:80, sex = c("male", "female"), year = proj.years))
  netmigr[1:80, , ] <- apply(netmigr5[1:16,,], 2:3, beers::beers_sub_ordinary)
  netmigr[81, , ] <- netmigr5[17, , ]

  if (adjust_u5mig) {

    if (is.null(sx)) {
      sx <- read_sx(pjnz)
    }

    u5prop <- array(dim = c(5, 2))
    u5prop[1, ] <- sx[1, , 1] * 2
    u5prop[2, ] <- sx[2, , 1] * u5prop[1, ]
    u5prop[3, ] <- sx[3, , 1] * u5prop[2, ]
    u5prop[4, ] <- sx[4, , 1] * u5prop[3, ]
    u5prop[5, ] <- sx[5, , 1] * u5prop[4, ]

    u5prop <- sweep(u5prop, 2, colSums(u5prop), "/")

    netmigr[1:5 , 1, ] <- u5prop[ , 1, drop = FALSE] %*% netmigr5[1, 1, ]
    netmigr[1:5 , 2, ] <- u5prop[ , 2, drop = FALSE] %*% netmigr5[1, 2, ]
  }

  netmigr
}

adjust_spectrum_netmigr <- function(netmigr) {

  ## Spectrum adjusts net-migration to occur half in
  ## current age group and half in next age group

  netmigr_adj <- netmigr
  netmigr_adj[-1,,] <- (netmigr[-1,,] + netmigr[-81,,])/2
  netmigr_adj[1,,] <- netmigr[1,,]/2
  netmigr_adj[81,,] <- netmigr_adj[81,,] + netmigr[81,,]/2

  netmigr_adj
}

#' Prepare leapfrog input parameters from Spectrum PJNZ
#'
#' @param pjnz path to PJNZ file
#'
#' @return list of demographic and HIV projection input parameters
#'
#' @examples
#' pjnz <- system.file(
#'   "pjnz/bwa_aim-adult-art-no-special-elig_v6.13_2022-04-18.PJNZ",
#'   package = "frogger")
#' parameters <- prepare_leapfrog_parameters(pjnz)
#'
#' @export
prepare_leapfrog_parameters <- function(pjnz) {
  ## TODO: We're reading the PJNZ file several times below, revisit this,
  ## we should only have to read this in once
  dp <- prepare_leapfrog_demp(pjnz)
  projp <- prepare_leapfrog_projp(pjnz)
  c(dp, projp)
}

#' Prepare demographic inputs from Spectrum PJNZ
#'
#' @param pjnz path to PJNZ file
#'
#' @return list of demographic input parameters
#'
#' @examples
#' pjnz <- system.file(
#'   "pjnz/bwa_aim-adult-art-no-special-elig_v6.13_2022-04-18.PJNZ",
#'   package = "frogger")
#' demp <- prepare_leapfrog_demp(pjnz)
#'
#' @export
prepare_leapfrog_demp <- function(pjnz) {

  demp <- eppasm::read_specdp_demog_param(pjnz)

  demp$projection_start_year <- read_start_year(pjnz)
  demp$Sx <- read_sx(pjnz)
  demp$netmigr <- read_netmigr(pjnz, sx = demp$Sx)

  births_sex_prop_male <- demp$srb / (demp$srb + 100)
  demp$births_sex_prop <- rbind(male = births_sex_prop_male,
                                female = 1 - births_sex_prop_male)

  ## normalise ASFR distribution
  demp$asfr <- sweep(demp$asfr, 2, demp$tfr / colSums(demp$asfr), "*")

  ## NOTE: Reading this to obtain the Spectrum version number
  ##       This is a lot of redundant effort.
  projp <- eppasm::read_hivproj_param(pjnz)
  if (!grepl("^[4-6]\\.[0-9]", projp$spectrum_version)) {
    stop(paste0("Spectrum version not recognized: ", projp$spectrum_version))
  }
  demp$projection_period <- if (projp$spectrum_version >= "6.2") {
    "calendar"
  } else {
    "midyear"
  }

  if (demp$projection_period == "midyear") {
    demp$netmigr_adj <- adjust_spectrum_netmigr(demp$netmigr)
  } else {
    demp$netmigr_adj <- demp$netmigr
  }

  demp
}


#' Prepare adult HIV projection parameters from Spectrum PJNZ
#'
#' @param pjnz path to PJNZ file
#' @param hiv_steps_per_year number of Euler integration steps per year; default 10
#' @param hTS number of HIV treatment stages; default 3 (0-5 months,
#'   6-11 months, 12+ months)
#'
#' @return list of HIV projection parameters
#'
#' @examples
#' pjnz <- system.file(
#'   "pjnz/bwa_aim-adult-art-no-special-elig_v6.13_2022-04-18.PJNZ",
#'   package = "frogger")
#' projp <- prepare_leapfrog_projp(pjnz)
#'
#' @export
prepare_leapfrog_projp <- function(pjnz, hiv_steps_per_year = 10L, hTS = 3) {

  projp <- eppasm::read_hivproj_param(pjnz)

  ## Hard coded to expand age groups 15-24, 25-34, 35-44, 45+ to
  ## single-year ages 15:80.
  ## Requires extension for coarse HIV age group stratification
  idx_expand_full <- rep(1:4, times = c(10, 10, 10, 36))
  idx_expand_coarse <- rep(1:4, times = c(3, 2, 2, 2))

  v <- list()
  v$incidinput <- eppasm::read_incid_input(pjnz)
  v$incidpopage <- attr(v$incidinput, "incidpopage")
  v$incrr_sex <- projp$incrr_sex

  v$artmx_timerr <- projp$artmx_timerr[c(1, 2, rep(3, hTS - 2)), ]

  ## ## ART eligibility and numbers on treatment

  v$art15plus_num <- projp$art15plus_num
  v$art15plus_isperc <- projp$art15plus_numperc == 1

  ## convert percentage to proportion
  v$art15plus_num[v$art15plus_isperc] <- v$art15plus_num[v$art15plus_isperc] / 100

  ## eligibility starts in projection year idx
  ## ## !! NOTE: from EPP-ASM; not yet implemented
  ## v$specpop_percelig <- rowSums(with(projp$artelig_specpop[-1,], mapply(function(elig, percent, year) rep(c(0, percent*as.numeric(elig)), c(year - proj_start, proj_end - year + 1)), elig, percent, year)))
  v$artcd4elig_idx <- findInterval(-projp$art15plus_eligthres, -c(999, 500, 350, 250, 200, 100, 50))

  ## Update eligibility threshold from CD4 <200 to <250 to account for additional
  ## proportion eligible with WHO Stage 3/4.
  v$artcd4elig_idx <- replace(v$artcd4elig_idx, v$artcd4elig_idx==5L, 4L)

  v$pw_artelig <- with(projp$artelig_specpop["PW",], rep(c(0, elig), c(year - projp$yr_start, projp$yr_end - year + 1)))  # are pregnant women eligible (0/1)

  ## ## !! NOTE: from EPP-ASM; not yet implemented
  ## ## percentage of those with CD4 <350 who are based on WHO Stage III/IV infection
  ## v$who34percelig <- who34percelig

  v$art_dropout_recover_cd4 <- if (projp$spectrum_version >= "6.14") {TRUE} else {FALSE}

  ## Convert input percent dropout in 12 months to an annual rate (Rob Glaubius email 25 July 2024)
  v$art_dropout_rate <- -log(1.0 - projp$art_dropout/100)

  proj_years <- as.integer(projp$yr_end - projp$yr_start + 1L)
  v$t_ART_start <- min(c(unlist(apply(v$art15plus_num > 0, 1, which)), proj_years))

  ## New ART patient allocation options
  v$art_alloc_method <- projp$art_alloc_method
  v$art_alloc_mxweight <- projp$art_prop_alloc[1]

  ## Scale mortality among untreated population by ART coverage
  v$scale_cd4_mort <- projp$scale_cd4_mort

  ## State space dimensions
  v$hAG_SPAN_full <- rep(1L, 66L)
  v$hAG_SPAN_coarse <- c(2L, 3L, 5L, 5L, 5L, 5L, 5L, 5L, 31L)

  ## Add in pediatric components
  v$cd4fert_rat <- projp$cd4fert_rat
  v$frr_art6mos <- projp$frr_art6mos
  v$frr_scalar <- projp$frr_scalar
  v$fert_rat <- projp$fert_rat
  ## HIV positive entrants, right now just doing those without ART
  v$age15hivpop <- projp$age15hivpop

  ## Use Beer's coefficients to distribution IRRs by age/sex
  v$incrr_age <- apply(projp$incrr_age, 2:3, beers_open_ended)[16:81, , ] ## !! Hard coded
  v$incrr_age[v$incrr_age < 0] <- 0

  v$cd4_initdist_full <- projp$cd4_initdist[ , idx_expand_full, ]
  v$cd4_prog_full <- (1-exp(-projp$cd4_prog[ , idx_expand_full, ] / hiv_steps_per_year)) * hiv_steps_per_year
  v$cd4_mort_full <- projp$cd4_mort[ ,idx_expand_full, ]
  v$art_mort_full <- projp$artmx_multiplier * projp$art_mort[c(1, 2, rep(3, hTS - 2)), , idx_expand_full, ]

  v$cd4_initdist_coarse <- projp$cd4_initdist[ , idx_expand_coarse, ]
  v$cd4_prog_coarse <- (1-exp(-projp$cd4_prog[ , idx_expand_coarse, ] / hiv_steps_per_year)) * hiv_steps_per_year
  v$cd4_mort_coarse <- projp$cd4_mort[ ,idx_expand_coarse, ]
  v$art_mort_coarse <- projp$artmx_multiplier * projp$art_mort[c(1, 2, rep(3, hTS - 2)), , idx_expand_coarse, ]

  v
}

#!/usr/bin/env false

function color {
	local color="$1"
	shift

	echo "$(tput setaf $color)$@$(tput sgr0)"
}

function color_err {
	local color="$1"
	shift

	echo 1>&2 "$(tput setaf $color)$@$(tput sgr0)"
}

# failure message
function __error_handing {
	local last_status_code="$1"
	local error_line_number="$2"
	local error_site="$(caller)"
	color_err 9 ""
	color_err 9 "-----------------------------------------------------------------------"
	color_err 9 ""
	color_err 9 "Error - exited with status $last_status_code at line $error_line_number (caller $error_site)."
	color_err 9 ""
	#perl -slne 'if($.+5 >= $ln && $.-4 <= $ln){ $_="$. $_"; s/$ln/">" x length($ln)/eg; s/^\D+.*?$/\e[1;31m$&\e[0m/g; print "$_"}' -- -ln="$error_line_number" "$0"
}

# success message
function bye {
	color 12 ""
	color 12 "-----------------------------------------------------------------------"
	color 12 ""
	color 12 "Success - Script execution done"
	exit 0
}

trap '__error_handing $? $LINENO' ERR

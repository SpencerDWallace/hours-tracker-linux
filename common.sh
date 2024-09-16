#!/bin/bash

sdir=$(cd $(dirname $BASH_SOURCE[0]) && pwd)
HT_OUT="${sdir}/hours"
HT_YEAR=$(date +"%y")
HT_YEAR_FOLDER="$HT_OUT/$HT_YEAR"
HT_TIME=$(date +"%T")
HT_DAY=$(date +"%d")
HT_MONTH=$(date +"%m")
[ ! -d  "$HT_OUT/$HT_YEAR" ] && mkdir -p "$HT_OUT/$HT_YEAR"

add_timestamp ()
{
local print_string=""
local bin_char=""

if [ "$1" = "i" ]
then
	print_string="in"
	bin_char="$1"
elif [ "$1" = "o" ]
then
	print_string="out"
	bin_char="$1"
else
	echo "invalid input."
	exit
fi

echo "$bin_char$HT_DAY#$HT_TIME" >> "$HT_OUT/$HT_YEAR/$HT_MONTH.txt"
echo "clocked $print_string at: $HT_MONTH/$HT_DAY/$HT_YEAR, $HT_TIME"
}

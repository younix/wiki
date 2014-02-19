#!/bin/sh
#
# prints all environment vars

printf "Content-Type: text/plain\n\n";

env

while read line; do
	echo $line
done

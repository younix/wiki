#!/bin/sh

MD=md/index.md

sed -e '/__MD_SRC__/ {' -e "r $MD" -e 'd' -e '}' template/webedit.html

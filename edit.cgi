#!/bin/sh

MD=md/index.md

cat template/webedit.html | sed -e "/__MD_SRC__/r $MD"

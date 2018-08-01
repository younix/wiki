#!/bin/sh

set -eu

page=$1
name=$(basename "${page%.html}")

cat > $page <<-EOF
	<!DOCTYPE html>
	<html>
	<head>
		<title>${name}</title>
		<link rel="stylesheet" type="text/css" href="/style.css" />
	</head>
	<body>
	<a href="/cgi-bin/edit?path=${name}">edit</a>
EOF

echo '<nav>' >> $page
ls $(dirname $page)/*.html | while read file; do
	fname=$(basename ${file%.html})
	cat >> $page <<-EOF
		<a href="$(basename "${file}")">${fname}</a>
	EOF
done
echo '</nav>' >> $page

cmark "${page%.html}.md" >> $page

cat >> $page <<-EOF
	</html>
EOF

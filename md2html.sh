#!/bin/sh

set -eu

page=$1
name=$(basename "${page%.html}")

cat > $page <<-EOF
	<!DOCTYPE html>
	<html>
	<head>
		<title>${name}</title>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width">
		<link rel="stylesheet" type="text/css" href="/style.css" />
	</head>
	<body>
EOF

echo '<aside>' >> $page
ls $(dirname $page)/*.html | sort | while read file; do
	fname=$(basename ${file%.html})
	cat >> $page <<-EOF
		<a href="$(basename "${file}")">${fname}</a>
	EOF
done
echo '</aside>' >> $page

echo '<main>' >> $page
cmark "${page%.html}.md" >> $page
echo '</main>' >> $page

cat >> $page <<-EOF
	<hr>
	<a href="/edit.cgi?path=${name}">edit</a>
	</body>
	</html>
EOF

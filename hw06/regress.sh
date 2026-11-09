#!/bin/sh
set -eu

actual=$(mktemp)
trap 'rm -f "$actual"' EXIT HUP INT TERM

for file in scenarios/*.txt; do
	[ -f "$file" ] || {
		echo "FAIL: no scenarios found" >&2
		exit 1
	}
	name=$(basename "$file" .txt)
	expected="expected/$name.txt"

	[ -f "$expected" ] || {
		echo "FAIL: missing $expected" >&2
		exit 1
	}

	./replay "$file" > "$actual"
	if diff -u "$expected" "$actual"; then
		echo "PASS: regression $name"
	else
		echo "FAIL: regression $name" >&2
		exit 1
	fi
done

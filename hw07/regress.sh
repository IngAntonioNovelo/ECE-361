
#!/bin/sh
set -eu

passed=0
failed=0

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT HUP INT TERM

for input in inputs/*.txt; do
	name=${input##*/}
	name=${name%.txt}

	actual="$tmpdir/$name.out"
	err="$tmpdir/$name.err"

	if ./balance < "$input" > "$actual" 2> "$err"; then
		actual_status=0
	else
		actual_status=$?
	fi

	expected_status=$(cat "expected/$name.status")

	if cmp -s "expected/$name.out" "$actual" &&
		[ "$actual_status" -eq "$expected_status" ] &&
		[ ! -s "$err" ]; then

		printf 'PASS  %s\n' "$name"
		passed=$((passed + 1))
	else
		printf 'FAIL  %s (expected status %s, got %s)\n' \
			"$name" "$expected_status" "$actual_status"

		if ! cmp -s "expected/$name.out" "$actual"; then
			diff -u "expected/$name.out" "$actual" || true
		fi

		if [ -s "$err" ]; then
			printf 'stderr:\n'
			cat "$err"
		fi

		failed=$((failed + 1))
	fi
done

printf 'Regression summary: %s passed, %s failed\n' \
	"$passed" "$failed"

[ "$failed" -eq 0 ]

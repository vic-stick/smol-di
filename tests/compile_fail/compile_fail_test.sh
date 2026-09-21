#!/bin/sh
set -eu

source_file="$1"
workspace_root="$(cd "$(dirname "$source_file")/../.." && pwd)"
output_file="$TEST_TMPDIR/compile_fail.out"

if /opt/homebrew/bin/g++-16 -std=c++2c -freflection \
    -I"$workspace_root/include" "$source_file" -o "$output_file" \
    >"$TEST_TMPDIR/compiler.stdout" 2>"$TEST_TMPDIR/compiler.stderr"; then
    echo "Expected $source_file to fail compilation, but it compiled."
    exit 1
fi

exit 0

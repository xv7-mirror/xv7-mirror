#!/bin/sh
# tools/sign.sh
# Rewrite of tools/sign.pl to Bash.
# Usage: ./tools/sign.sh <bootblock>

if [ $# -ne 1 ]; then
    echo "Usage: $0 <bootblock>" >&2
    exit 1
fi

FILE="$1"

if [ ! -r "$FILE" ]; then
    echo "open $FILE: No such file or directory" >&2
    exit 1
fi

n=$(wc -c < "$FILE")

if [ "$n" -gt 510 ]; then
    echo "boot block too large: $n bytes (max 510)" >&2
    exit 1
fi

echo "boot block is $n bytes (max 510)" >&2

padding=$((510 - n))

tmpfile=$(mktemp)

trap 'rm -f "$tmpfile"' EXIT

(
    cat "$FILE"
    dd if=/dev/zero bs=1 count=$padding 2>/dev/null
    printf "\125\252"
) > "$tmpfile"

mv "$tmpfile" "$FILE"

trap - EXIT
exit 0

#!/usr/bin/env bash
DIR="$(cd "$(dirname "$0")" && pwd)"
ARGS=("$@")

for i in "${!ARGS[@]}"; do
  if [ "${ARGS[$i]}" = "gen" ]; then
    ARGS[$i]="gmake"
  fi
done

"$DIR/premake5" --file="$DIR/premake5.lua" "${ARGS[@]}"
status=$?

if [ "$status" -eq 0 ] && [ ! -f "$DIR/Makefile.custom" ]; then
  exit "$status"
fi

if [ "$status" -eq 0 ] && ! grep -q '\-include Makefile.custom' "$DIR/Makefile" 2>/dev/null; then
  printf '\n-include Makefile.custom\n' >> "$DIR/Makefile"
fi

exit "$status"

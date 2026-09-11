#!/usr/bin/env bash
set -euo pipefail

if ! command -v aseprite >/dev/null 2>&1; then
  printf '%s\n' "aseprite was not found. Install Aseprite or export the file manually." >&2
  exit 1
fi

if [ "$#" -ne 2 ]; then
  printf '%s\n' "usage: tools/export_aseprite.sh input.aseprite output.png" >&2
  exit 2
fi

aseprite --batch "$1" --save-as "$2"

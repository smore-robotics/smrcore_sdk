#!/usr/bin/env bash
# Byte-compile every Python example under examples_py/ (recursively).
#
# This only checks that the examples parse and compile; it does NOT run them and
# does NOT require a live robot or the rcore_sdk wheel to be installed.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd -P)"
cd "$ROOT_DIR"

find examples_py -name '*.py' -print0 | xargs -0 python3 -m py_compile
echo "Python examples compile: ok"

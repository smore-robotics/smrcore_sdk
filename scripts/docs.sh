#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd -P)"
cd "$ROOT_DIR"

if [ ! -f "3rdparty/smrcore_sdk/include/rcore/sdk/robot.hpp" ]; then
    echo "SDK not found at 3rdparty/smrcore_sdk; running ./scripts/download.sh"
    ./scripts/download.sh
fi

doxygen doc/Doxyfile

echo
echo "Local documentation (served under the site_url base path):"
echo "  http://127.0.0.1:8000/smrcore_sdk/"
echo "  http://127.0.0.1:8000/smrcore_sdk/zh/"
echo "  http://127.0.0.1:8000/smrcore_sdk/api/cpp/index.html"
echo
echo "GitHub Pages URL after deployment:"
echo "  https://smore-robotics.github.io/smrcore_sdk/"
echo "  https://smore-robotics.github.io/smrcore_sdk/zh/"
echo "  https://smore-robotics.github.io/smrcore_sdk/api/cpp/index.html"
echo

exec "${MKDOCS_BIN:-mkdocs}" serve

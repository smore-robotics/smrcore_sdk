#!/usr/bin/env bash
set -e

GITHUB_REPO="smore-robotics/smrcore_sdk"
VERSION="${VERSION:-latest}"
INSTALL_DIR="3rdparty/smrcore_sdk"

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd -P)"

if [ "$VERSION" = "latest" ]; then
    LATEST_URL="$(curl -Ls -o /dev/null -w '%{url_effective}' "https://github.com/${GITHUB_REPO}/releases/latest")"
    VERSION="${LATEST_URL##*/}"
    VERSION="${VERSION#v}"
else
    VERSION="${VERSION#v}"
fi

if ! printf '%s\n' "$VERSION" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
    echo "download: VERSION must be latest or x.y.z" >&2
    exit 1
fi

URL="https://github.com/${GITHUB_REPO}/releases/download/v${VERSION}/smrcore_sdk-cpp-linux-x86_64-v${VERSION}.tar.gz"

ASSET="${URL##*/}"

cd "$ROOT_DIR"

echo "Downloading ${ASSET}"
rm -f "$ASSET"
curl -L --fail --show-error "$URL" -o "$ASSET"

echo "Extracting to ${INSTALL_DIR}"
rm -rf "$INSTALL_DIR"
mkdir -p "$INSTALL_DIR"
tar -xf "$ASSET" -C "$INSTALL_DIR"
rm -f "$ASSET"

echo "Done: ${ROOT_DIR}/${INSTALL_DIR}"

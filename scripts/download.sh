#!/usr/bin/env bash
set -e

VERSION="0.0.1"
GITHUB_REPO="smore-robotics/smrore_sdk"
INSTALL_DIR="3rdparty/smrcore_sdk"

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd -P)"
ASSET="smrcore_sdk-cpp-linux-x86_64-v${VERSION}.tar.gz"
URL="https://github.com/${GITHUB_REPO}/releases/download/v${VERSION}/${ASSET}"

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

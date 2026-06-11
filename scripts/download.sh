#!/usr/bin/env bash
set -e

GITHUB_REPO="smore-robotics/smrcore_sdk"
INSTALL_DIR="3rdparty/smrcore_sdk"

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd -P)"

# Version resolution priority: SDK_VERSION > VERSION > .sdk-version.
# VERSION=latest is still supported and resolves the latest GitHub release.
SDK_VERSION="${SDK_VERSION:-${VERSION:-}}"
if [ -z "$SDK_VERSION" ] && [ -f "${ROOT_DIR}/.sdk-version" ]; then
    SDK_VERSION="$(tr -d '[:space:]' < "${ROOT_DIR}/.sdk-version")"
fi

if [ -z "$SDK_VERSION" ]; then
    echo "download: set SDK_VERSION/VERSION or provide .sdk-version" >&2
    exit 1
fi

if [ "$SDK_VERSION" = "latest" ]; then
    LATEST_URL="$(curl -Ls -o /dev/null -w '%{url_effective}' "https://github.com/${GITHUB_REPO}/releases/latest")"
    SDK_VERSION="${LATEST_URL##*/}"
fi
SDK_VERSION="${SDK_VERSION#v}"

if ! printf '%s\n' "$SDK_VERSION" | grep -Eq '^[0-9]+\.[0-9]+\.[0-9]+$'; then
    echo "download: SDK_VERSION must be x.y.z (or VERSION=latest), got: ${SDK_VERSION}" >&2
    exit 1
fi

# Release tag to download from. Defaults to the official release tag for the
# version; CI can set SDK_RELEASE_TAG=prerelease to use candidate assets.
SDK_RELEASE_TAG="${SDK_RELEASE_TAG:-v${SDK_VERSION}}"

URL="https://github.com/${GITHUB_REPO}/releases/download/${SDK_RELEASE_TAG}/smrcore_sdk-cpp-linux-x86_64-v${SDK_VERSION}.tar.gz"

ASSET="${URL##*/}"

cd "$ROOT_DIR"

echo "Downloading ${ASSET} from release ${SDK_RELEASE_TAG}"
rm -f "$ASSET"
curl -L --fail --show-error "$URL" -o "$ASSET"

echo "Extracting to ${INSTALL_DIR}"
rm -rf "$INSTALL_DIR"
mkdir -p "$INSTALL_DIR"
tar -xf "$ASSET" -C "$INSTALL_DIR"
rm -f "$ASSET"

echo "Done: ${ROOT_DIR}/${INSTALL_DIR}"

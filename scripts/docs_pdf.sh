#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd -P)"
cd "$ROOT_DIR"

SITE_DIR="${SITE_DIR:-site}"
PDF_DIR="${PDF_DIR:-$SITE_DIR/pdf}"
WORK_DIR="$PDF_DIR/.parts"
OUT_PDF="${OUT_PDF:-$PDF_DIR/smrcore_sdk_zh.pdf}"

CHROME_BIN="${CHROME_BIN:-}"
if [ -z "$CHROME_BIN" ]; then
    for candidate in google-chrome chromium chromium-browser; do
        if command -v "$candidate" >/dev/null 2>&1; then
            CHROME_BIN="$(command -v "$candidate")"
            break
        fi
    done
fi

if [ -z "$CHROME_BIN" ]; then
    echo "docs_pdf: missing Chrome/Chromium; install google-chrome or chromium" >&2
    exit 1
fi

for cmd in mkdocs doxygen pdfunite python3; do
    if ! command -v "$cmd" >/dev/null 2>&1; then
        echo "docs_pdf: missing required command: $cmd" >&2
        exit 1
    fi
done

if [ ! -f "3rdparty/smrcore_sdk/include/rcore/sdk/robot.hpp" ]; then
    echo "SDK not found at 3rdparty/smrcore_sdk; running ./scripts/download.sh"
    ./scripts/download.sh
fi

echo "-> generating Doxygen C++ API HTML"
doxygen doc/Doxyfile

echo "-> building MkDocs site"
mkdocs build --clean --site-dir "$SITE_DIR"

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR" "$PDF_DIR"

file_uri() {
    python3 - "$1" <<'PY'
from pathlib import Path
import sys
print(Path(sys.argv[1]).resolve().as_uri())
PY
}

render_pdf() {
    local html="$1"
    local pdf="$2"
    local uri
    uri="$(file_uri "$html")"
    "$CHROME_BIN" \
        --headless \
        --disable-gpu \
        --no-sandbox \
        --disable-dev-shm-usage \
        --run-all-compositor-stages-before-draw \
        --virtual-time-budget=1000 \
        --no-pdf-header-footer \
        --print-to-pdf-no-header \
        "--print-to-pdf=$pdf" \
        "$uri" >/dev/null 2>&1
}

add_page() {
    local rel="$1"
    local path="$SITE_DIR/$rel"
    if [ -f "$path" ]; then
        PAGES+=("$path")
    else
        echo "docs_pdf: skip missing page: $rel" >&2
    fi
}

# Generate the cover and table-of-contents pages from templates. These are
# standalone print pages (not MkDocs pages) so they carry no site chrome.
ASSET_DIR="scripts/pdf/assets"
COVER_HTML="$WORK_DIR/cover.html"
TOC_HTML="$WORK_DIR/toc.html"
VERSION_HPP="3rdparty/smrcore_sdk/include/rcore/sdk/version.hpp"
SDK_VERSION="$(grep -oE 'RCORE_SDK_VERSION_STRING "[0-9.]+"' "$VERSION_HPP" 2>/dev/null \
    | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || true)"
[ -z "$SDK_VERSION" ] && SDK_VERSION="dev"
BUILD_DATE="$(date '+%Y.%m')"
LOGO_PATH="$ROOT_DIR/docs/assets/logo.png"

gen_asset() {
    sed -e "s|{{LOGO}}|$LOGO_PATH|g" \
        -e "s|{{VERSION}}|$SDK_VERSION|g" \
        -e "s|{{DATE}}|$BUILD_DATE|g" \
        "$1" >"$2"
}
gen_asset "$ASSET_DIR/cover.html" "$COVER_HTML"
gen_asset "$ASSET_DIR/toc.html" "$TOC_HTML"

PAGES=()

# Cover + table of contents.
PAGES+=("$COVER_HTML")
PAGES+=("$TOC_HTML")

# Chinese guide pages.
add_page "zh/index.html"
add_page "zh/install/index.html"
add_page "zh/usage/index.html"
add_page "zh/examples/index.html"
add_page "zh/examples_basics/index.html"
add_page "zh/examples_motion/index.html"
add_page "zh/examples_config/index.html"
add_page "zh/examples_compliance/index.html"
add_page "zh/support/index.html"
add_page "zh/api/index.html"

# Doxygen API pages. Keep generated source/member-list/search pages out of the
# combined PDF; they make the file much larger without helping a first review.
API_DIR="$SITE_DIR/api/cpp"
if [ -d "$API_DIR" ]; then
    for rel in \
        "api/cpp/index.html" \
        "api/cpp/annotated.html" \
        "api/cpp/classes.html" \
        "api/cpp/files.html" \
        "api/cpp/namespaces.html"; do
        add_page "$rel"
    done

    while IFS= read -r api_page; do
        PAGES+=("$api_page")
    done < <(
        find "$API_DIR" -maxdepth 1 -type f \
            \( -name 'class*.html' -o -name 'struct*.html' -o -name 'namespace*.html' -o -name '*_8hpp.html' \) \
            ! -name 'classes.html' \
            ! -name 'namespaces.html' \
            ! -name '*-members.html' \
            ! -name '*_source.html' \
            ! -path '*/search/*' \
            | sort
    )
fi

if [ "${#PAGES[@]}" -eq 0 ]; then
    echo "docs_pdf: no pages found" >&2
    exit 1
fi

PARTS=()
MANIFEST="$WORK_DIR/manifest.tsv"
: >"$MANIFEST"
idx=0
for page in "${PAGES[@]}"; do
    idx=$((idx + 1))
    part="$(printf "%s/%04d.pdf" "$WORK_DIR" "$idx")"
    rel="${page#$SITE_DIR/}"
    echo "-> [$idx/${#PAGES[@]}] $rel"
    render_pdf "$page" "$part"
    PARTS+=("$part")
    printf '%s\t%s\n' "$part" "$page" >>"$MANIFEST"
done

echo "-> merging ${#PARTS[@]} PDF parts"
pdfunite "${PARTS[@]}" "$OUT_PDF"

if python3 -c "import pypdf" >/dev/null 2>&1; then
    echo "-> writing PDF bookmarks"
    python3 scripts/pdf/bookmarks.py "$OUT_PDF" "$MANIFEST"
else
    echo "docs_pdf: pypdf not installed; skipping bookmarks (pip install --user pypdf)" >&2
fi

# Stamp a footer page number on content pages (cover + TOC stay clean).
if python3 -c "import pypdf, reportlab" >/dev/null 2>&1; then
    echo "-> stamping page numbers"
    python3 scripts/pdf/stamp.py "$OUT_PDF" 2 "$LOGO_PATH"
else
    echo "docs_pdf: reportlab not installed; skipping page numbers (pip install --user reportlab)" >&2
fi

echo "-> done: $OUT_PDF"

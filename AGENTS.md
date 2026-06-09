# AGENTS.md — smrcore_sdk

Public documentation, examples, and C++ API reference for the **Smartmore Robot
SDK**. This repository does **not** contain SDK source code — it consumes
versioned release assets (C++ tarballs + Python wheel) downloaded from GitHub
Releases.

## Layout

| Path | What |
|---|---|
| `examples/` | C++ examples (basics, motion, config, compliance) |
| `examples_py/` | Python examples (mirror of the C++ set) |
| `docs/` | MkDocs site, bilingual (en + `*.zh.md` via mkdocs-static-i18n) |
| `scripts/` | `download.sh`, `build.sh`, `docs_pdf.sh`, `check_python_examples.sh` |
| `scripts/pdf/` | PDF cover/toc/divider templates + `bookmarks.py`, `stamp.py` |
| `.github/workflows/` | `ci.yml` (build/test examples), `release-pdf.yml` (docs PDF) |

## Common tasks

```bash
./scripts/download.sh              # fetch the C++ SDK (VERSION=x.y.z to pin)
./scripts/build.sh                 # build the C++ examples
mkdocs serve                       # preview the docs site
mkdocs build --strict              # build docs (must pass with no warnings)
./scripts/docs_pdf.sh              # build the Chinese docs PDF
```

`docs_pdf.sh` needs Chrome/Chromium, `doxygen`, `graphviz`, poppler-utils, and
`pip install pypdf reportlab`.

## Conventions

- **Release asset names:** `smrcore_sdk-cpp-<os>-x86_64-v<version>.tar.gz`,
  `rcore_sdk_py-<version>-<python-tags>.whl`,
  `smrcore_sdk-docs-zh-v<version>.pdf`.
- **Docs version snippets** use `VERSION=0.0.1  # replace as needed` — they are
  illustrative and are *not* edited per release.
- **Bilingual docs:** every `docs/foo.md` has a matching `docs/foo.zh.md`. Add
  new nav entries to both `nav` and `nav_translations` in `mkdocs.yml`.
- **C++ function naming:** PascalCase.
- **PDF structure** is driven by the page order in `scripts/docs_pdf.sh` plus
  `scripts/pdf/bookmarks.py`: divider pages become the top-level bookmarks
  (用户指南 / C++ 示例指南 / Python 示例指南 / C++ API 参考). The PDF name and
  version come from `version.hpp` in the downloaded SDK.

## Releasing

This public repository treats GitHub Releases as the source of published SDK
assets. For each SDK version, confirm the release contains the expected public
assets:

- `smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz`
- `smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz`
- Linux Python wheel, for example `rcore_sdk_py-<version>-cp310-cp310-linux_x86_64.whl`
- Windows Python wheel, for example `rcore_sdk_py-<version>-cp310-cp310-win_amd64.whl`
- `smrcore_sdk-docs-zh-v<version>.pdf`

From this repo's side, the documentation PDF is produced by `release-pdf.yml`.
It downloads the public SDK archive, builds `smrcore_sdk-docs-zh-v<version>.pdf`,
verifies there are no local-path leaks or PDF link annotations, and attaches the
PDF to the GitHub Release. Re-run it for an existing tag with:

```bash
gh workflow run release-pdf.yml -f version=x.y.z
```

Before considering a release complete, check:

1. GitHub Release tag is `v<version>` and all asset filenames use the same
   version.
2. `./scripts/download.sh` succeeds with the default latest release.
3. `VERSION=x.y.z ./scripts/download.sh` succeeds for the new version.
4. `./scripts/build.sh` builds all C++ examples.
5. `./scripts/check_python_examples.sh` succeeds.
6. GitHub Actions `CI` is green on Linux and Windows.
7. GitHub Actions `Release PDF` is green and the release contains the generated
   PDF.
8. `mkdocs build --strict` succeeds.
9. Public docs do not mention private infrastructure, private repositories,
   unpublished package locations, access tokens, local absolute paths, or
   implementation details that are not already part of the public SDK/API.

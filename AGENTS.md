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
| `.sdk-version` | SDK version (`x.y.z`) the docs/examples currently target |
| `.github/workflows/` | `ci.yml` (PR candidate validation + main checks), `release-pdf.yml` (docs PDF), `release-smoke.yml` (official release verification) |

## Common tasks

```bash
./scripts/download.sh              # fetch the C++ SDK pinned by .sdk-version
VERSION=x.y.z ./scripts/download.sh            # pin a specific released version
SDK_RELEASE_TAG=prerelease ./scripts/download.sh   # candidate assets (PR stage)
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
- **Docs version snippets** use `VERSION=0.0.3  # replace as needed` — they are
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
assets. There are two release channels:

- **`prerelease`** — a single rolling GitHub pre-release that holds *candidate*
  assets for the version in `.sdk-version`. Only PR CI downloads from it.
  Asset filenames always use the real version (e.g. `0.0.2`), never
  `prerelease`.
- **`v<version>`** — the official release users consume.

Expected official release assets:

- `smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz`
- `smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz`
- Linux Python wheel, for example `rcore_sdk_py-<version>-cp310-cp310-linux_x86_64.whl`
- Windows Python wheel, for example `rcore_sdk_py-<version>-cp310-cp310-win_amd64.whl`
- `smrcore-simulator-linux-x86_64-v<version>.tar.gz` (local simulator;
  lets users run examples without hardware)
- `smrcore_sdk-docs-zh-v<version>.pdf`

The simulator tarball embeds a `VERSION` file that must equal the release
version; the internal sync scripts verify it and abort on mismatch. The
simulator is built from the rcore repository tagged with the **same**
`x.y.z` version as the SDK.

### Release order (avoid the PR/release deadlock)

1. Internal pipelines produce the SDK assets **and the Linux simulator**
   for the new version. The rcore repository must be tagged with the same
   `x.y.z` so the simulator's embedded `VERSION` matches.
2. Upload candidate assets to the `prerelease` release (internal
   `sync_smrcore_prerelease_to_github.sh`, not in this repo). The sync
   expects 5 assets (C++ tarball x2 + wheel x2 + Linux simulator) and
   verifies the simulator's embedded `VERSION`.
3. Open a PR here updating `.sdk-version`, docs, and examples.
4. PR CI (`ci.yml`) reads `.sdk-version` and downloads candidate assets from
   `prerelease` to build/verify on Linux and Windows. On Linux it also runs
   the stable example subset (the `EXAMPLES` list inside
   `scripts/run_examples_against_sim.sh`) against the
   candidate simulator (skips with a warning only if the simulator asset is
   absent from `prerelease`).
5. Merge the PR. Pushes to `main` only run lightweight checks (docs build,
   script syntax, `py_compile`) — they never depend on release assets.
6. Tag the merge commit on `main` as `v<version>` and push the tag.
7. Sync official assets to the `v<version>` release (internal
   `sync_smrcore_release_to_github.sh`; it refuses to run unless the tag
   exists and points at `main`).
8. `release-pdf.yml` checks out the `v<version>` tag, builds
   `smrcore_sdk-docs-zh-v<version>.pdf`, verifies no local-path leaks or PDF
   link annotations, and attaches it to the release. Re-run with:

   ```bash
   gh workflow run release-pdf.yml -f version=x.y.z
   ```

9. `release-smoke.yml` verifies the official release end to end:

   ```bash
   gh workflow run release-smoke.yml -f version=x.y.z
   ```

10. Candidate assets stay on `prerelease` until the next cycle overwrites
    them (the prerelease sync script removes stale assets first).

Before considering a release complete, check:

1. GitHub Release tag is `v<version>`, it points at `main`, and all asset
   filenames use the same version.
2. `.sdk-version` on `main` matches the released version.
3. `./scripts/download.sh` succeeds (defaults to `.sdk-version`).
4. `VERSION=x.y.z ./scripts/download.sh` succeeds for the new version.
5. `./scripts/build.sh` builds all C++ examples.
6. `./scripts/check_python_examples.sh` succeeds.
7. GitHub Actions `CI` was green on the PR (Linux and Windows).
8. GitHub Actions `Release PDF` is green and the release contains the
   generated PDF.
9. GitHub Actions `Release Smoke` is green for the new version. Its Linux
   job downloads the released simulator, checks the embedded `VERSION`, and
   runs the stable example subset against it — this is the real
   SDK <-> simulator pairing proof for the release.
10. `mkdocs build --strict` succeeds.
11. Public docs do not mention private infrastructure, private repositories,
    unpublished package locations, access tokens, local absolute paths, or
    implementation details that are not already part of the public SDK/API.

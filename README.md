---
description: "Font-friendly Office-to-PDF conversion in Node.js, with prebuilt LibreOffice engines."
kind: "package-library"
---
# @deepseek-ai/libreoffice-kit

English | [中文](README.zh.md)

---

## Branch Notes: libreoffice-kit-0.2.3-linux-x64

### Overview

This branch is created from upstream `deepseek-ai/dsh-libreoffice-kit` (master branch, version 0.1.3) with the following goals:

1. Upgrade the repository from **0.1.3 to 0.2.3**
2. Add **linux-x64-glibc native engine** support (approach: upstream official compilation)
3. Complete source compilation via **GitHub Actions cloud workflow**

### Changes

#### 1. Build scripts upgraded to 0.2.3

**Source of 0.2.3**: Extracted from the `@deepseek-ai/libreoffice-kit-win32-x64@0.2.3` platform package published by upstream deepseek-ai on npm. The `sources/` directory of this package contains the complete recipe (build scripts, patches, configuration) used by DeepSeek upstream to compile the LibreOffice native engine. The package was downloaded via `npm pack`, unpacked, and its `sources/` directory was extracted to overwrite the local `engine/native/` and related directories. LibreOffice core source is pinned to revision `bce0998`.

Detailed changes:

- `engine/native/` — 12 new source files (editor, graphite, ganesh, slideshow)
- `engine/native/patches/` — 28 new patches (0066-0092)
- `engine/native/patches/0093-linux-lpsolve-static-colamd.patch` — fixes `$colamd_c` variable not expanding in lpsolve static build
- `engine/wasm-source/patches/` — 1 new patch
- `scripts/` — 10 updated build scripts
- `packages/frame-forwarding/` — new WebGPU frame forwarding SDK

#### 2. Version bump

| Package | Old | New |
|---------|-----|-----|
| Root workspace | 0.1.3 | 0.2.3 |
| entry (@deepseek-ai/libreoffice-kit) | 0.1.3 | 0.2.3 |
| win32-x64 / win32-arm64 | 0.1.3 | 0.2.3 |
| darwin-x64 / darwin-arm64 | 0.1.3 | 0.2.3 |
| wasm | 0.1.3 | 0.2.3 |
| linux-x64-glibc | 0.1.1 | 0.2.3 |
| linux-arm64-glibc | 0.1.1 | 0.2.3 |

#### 3. linux-x64-glibc native engine support

- `packages/entry/package.json` `optionalDependencies` adds `linux-x64-glibc`, `linux-arm64-glibc`, `koffi 3.1.1`
- `packages/linux-x64-glibc/prebuilds.json`:
  - `executable` changed from `bin/libreoffice-kit` to `bin/dsoffice`
  - `programDirectory` changed from `program` to `program/program`
  - `status` is `unbuilt` (updated to `built` after CI compilation)

#### 4. GitHub Actions workflow

- `.github/workflows/build-linux-x64-native.yml` — runs upstream official compilation on ubuntu-24.04 runner

### Build flow

```
checkout-core.mjs → build-native.mjs (patches + autogen + make) → stage-native.mjs
```

1. `checkout-core.mjs` fetches LibreOffice core source (revision `bce0998`)
2. `build-native.mjs` applies 93 patches, runs `autogen.sh` + `make build`
3. `stage-native.mjs` stages build output to `packages/linux-x64-glibc/`

#### Linux static build configuration

- `--disable-dynamic-loading` (static linking, `DISABLE_DYNLOADING=TRUE`)
- `--disable-skia` (no GPU rendering)
- `--disable-gui`, `--without-x` (headless mode)
- `--disable-nss`, `--with-tls=openssl`

---

## Current goal

**Font-friendly Office conversion and rendering in Node.js.** Version `0.1.0` extends the stable `0.0.1` line with format conversion, spreadsheet recalculation, a CLI, direct PNG rendering, and shared converter-factory font caches while retaining the native engine packages.

The priorities are document layout and readable text, explicit control over available fonts and substitutions, and an engine that applications can bundle and run offline. Size reduction serves that goal: retain the import, layout, drawing, and PDF-export machinery these documents need while removing unrelated desktop features and resources.

The API supports Office export and recalculation plus direct PNG rendering of Office and PDF inputs. Use it in a Node.js service, a desktop application, or a document-processing job. Applications manage their own authorization, storage, and preview interfaces.

Binary `.doc`, `.xls`, and `.ppt` support covers OLE compound documents such as Office 97–2003. RTF/HTML files renamed to these suffixes and `.wps` are not accepted. `missingFonts` reports declarations found in OOXML; binary formats return an empty list while LibreOffice performs font matching.

## Quick start

Requires Node.js **22.19.0 or newer**. Install the `0.1.3` package with its optional engines:

```sh
npm install @deepseek-ai/libreoffice-kit@0.1.3
```

The package installs the `dsoffice` command. From a local installation:

```sh
npm exec -- dsoffice render --input report.docx --output-dir report-pages --pages 1
npm exec -- dsoffice convert --input report.docx --output report.pdf
npm exec -- dsoffice recalculate --input budget.xlsx --output recalculated.xlsx
```

```js
import { createConverter } from '@deepseek-ai/libreoffice-kit';

const converter = await createConverter({
  timeoutMs: 120_000,
  // Omit to discover fonts in conventional system/user directories.
  // fontDirectories: ['/absolute/path/to/fonts'],
});
try {
  const result = await converter.render({
    inputPath: '/private/work/document.docx',
    outputPath: '/private/work/document.pdf',
  });
  console.log(result.backend, result.missingFonts);
} finally {
  await converter.dispose();
}
```

Supply absolute paths in caller-owned private directories; the output must not already exist. The PDF is written to `outputPath`, and `render` returns the selected backend and missing-font names. Each render owns a fresh native process or Node worker and private profile. Renders on one converter are serialized; cancellation and disposal wait for engine exit and cleanup. See the [Node API](packages/entry/README.md) for cancellation, errors, and resource limits.

## What “font-friendly” means in 0.1.0

- **Use available fonts.** The API discovers conventional system/user font directories, or indexes the roots supplied through `fontDirectories`. Custom roots replace the default list. `fontkit` reads font metadata and glyph coverage; selected files are passed to the engine as original font bytes.
- **Preserve requested families when available.** Exact installed families take priority, including handwriting and decorative fonts. WASM font requests also carry weight and italic information so matching installed faces can be selected; the catalog can supply additional fonts for missing glyphs.
- **Make substitutions configurable.** Default `fontFallbacks` cover common Latin and Simplified Chinese families, including Carlito for missing Calibri and Caladea for missing Cambria. Caller-supplied groups replace the defaults. These preferences only select available fonts; they do not install them. See the [default groups](packages/entry/src/options.ts).
- **Expose missing families and font budgets.** `missingFonts` reports unavailable families declared in readable document XML. `maxFontFiles`, `maxFontFileBytes`, and `maxLoadedFontBytes` bound indexing and explicit imports. Each operation rediscovers and revalidates fonts. Unchanged file metadata is shared in memory and in a bounded user-local disk cache; see [cache options and invalidation](packages/entry/README.md#font-metadata-cache).

No font collection is bundled or downloaded. Deployments supply fonts appropriate to their documents and redistribution rights; minimal containers need fonts installed or a configured font directory. WASM uses only imported fonts and rejects conversion with `unavailable` if none are usable. Native macOS and Windows can also use OS-managed fonts.

This improves control over font choice, but does not guarantee identical output to Microsoft Office or between engines. Native LibreOffice resolves installed originals and metric-compatible families before configured substitutions, and handles face selection itself. Font metrics can change line breaks and pagination; `missingFonts` is not a complete missing-glyph report. Comparing engines requires identical document bytes, fonts, and export options.

## Engines and distribution

The [Node package manifest](packages/entry/package.json) declares the engines for `0.1.0`:

| Engine | Role |
| --- | --- |
| `@deepseek-ai/libreoffice-kit-darwin-arm64` | Native helper for macOS on Apple Silicon. |
| `@deepseek-ai/libreoffice-kit-darwin-x64` | Native helper for Intel macOS. |
| `@deepseek-ai/libreoffice-kit-win32-arm64` | Native helper for Windows ARM64; requires ARM64 Node.js and the Microsoft Visual C++ v14 ARM64 Redistributable. |
| `@deepseek-ai/libreoffice-kit-win32-x64` | Native helper for Windows x64; requires the Microsoft Visual C++ v14 x64 Redistributable. |
| `@deepseek-ai/libreoffice-kit-wasm` | Shared Node WASM engine for Linux. |

Other native directories are development recipes, not additional released targets. The shared WASM package declares Linux as its npm OS, with no CPU or libc restriction; that declaration alone does not certify every Linux host. Native and WASM engines perform layout and PDF serialization on the CPU.

npm installs only the matching native package on macOS/Windows ARM64 or x64, and WASM on Linux. macOS and Windows require their native package and never fall back to WASM. Linux uses WASM unless an explicitly installed development native package supports its glibc version. A missing required engine or a corrupt installed engine rejects `createConverter` with `unavailable`; conversion failures do not switch engines.

The Node API and engines share the kit version. Installation uses prepared packages; no install hook or conversion compiles LibreOffice, downloads extra engines, or discovers the user's LibreOffice installation. npm distributes standard `.tgz` packages. GitHub Release engine downloads use verified XZ transfer archives for application builders to prepare and bundle. See [packaging](docs/packaging.md) and [release procedures](docs/building.md) for these distribution paths and installed-conversion checks.

## What was reduced, and why

The recipes reduce build components, linked code, installed resources, and transfer size separately:

| Layer | Changes in the code | Reason |
| --- | --- | --- |
| Native build | Disable Java/Python, scripting and extensions, Base database connectivity, PDF import, help/dictionaries, galleries/templates/icon themes, remote control, updates, and unused curl/WebDAV/CMIS/LDAP integrations. | Local OOXML → PDF needs document import and PDF export; desktop automation, database access, PDF input, and online services add dependencies outside that path. |
| Native linkage | Statically link Core on every native target, restrict exports, and discard unreachable code. System libraries and the Windows CRT remain dynamic. | Reduce code retained by shared-library export boundaries. |
| Native payload | Remove the verified duplicate macOS `urelibs` alias, SDK tools, launchers, Quick Look/Spotlight resources, disabled libraries, Basic/Python scripts, notebookbars, menus, and toolbars. Strip nonessential symbols while preserving dynamic exports; restore and verify macOS signatures. | Avoid shipping duplicate libraries and desktop/development resources in an application conversion engine. |
| WASM build and payload | Build Writer, Calc, and Impress for headless Node workers, with Java/Python, bundled fonts, OpenCL/OpenGL, and Skia disabled. Prune icon archives, notebookbars, menus/toolbars, Android sample documents, splash images, and shell resources from `soffice.data`. | Keep the document engines and CPU rendering path while reducing resources loaded into the WASM filesystem. The resource-pruning pass preserves retained bytes and metadata, regenerates offsets, and leaves the loader and compiled module unchanged. |
| Font payload | Omit bundled font collections; load original font files supplied by the deployment at runtime. | Avoid a fixed font payload and let applications choose the fonts needed for their documents, including CJK coverage and Office-compatible alternatives. |
| Release transfer | Use XZ for GitHub Release engine downloads, with separate size/hash checks for the compressed envelope and exact installation tar. | Reduce transfer bytes without changing installed engine contents. This is separate from payload pruning and from npm's `.tgz` format. |

The authoritative recipes are [native configuration](engine/native/configure.mjs), [native payload pruning](scripts/slim-native.mjs), [WASM configuration](engine/wasm-source/autogen.input), and [WASM resource pruning](engine/wasm-source/slim.mjs).

Native payload pruning removes XSLT resources and their filter registry, plus `CREDITS.fodt`. Installation `LICENSE.html` copies are removed only when byte-identical third-party notices remain under `licenses/`. The removed XSLT formats include Word 2003 XML, SpreadsheetML, UOF, DocBook, and XHTML; binary Office and OOXML conversion filters remain.

Windows statically links OpenSSL and disables NSS/GPGME. Payload pruning removes linked archives, scanner/GPG helpers, MSI installers, Shell extensions, ActiveX/SharePoint integrations, .NET CLI bindings, desktop launchers, Python wizards, and branding images. Static service registration omits desktop accessibility and OLE server registration. The conversion helper, selected UNO components, and runtime `.ini` files remain; see [packaging](docs/packaging.md).

Writer, Calc, Impress, binary Office and OOXML filters, PDF export, PDFium for embedded PDF/EMF graphics, shared layout/drawing libraries, charts, ICU and language resources remain. The native `en-US` build language selects UI resources; it does not restrict document text to English. Required runtime configuration and some UI resources remain because document services still use them. Matching source recipes, patches, hashes, and license notices travel with every engine package. These retained dependencies explain why the result is still a substantial document engine.

### Recorded size and fidelity checks

The existing local candidate measurements compare the preceding `0.1.2` gzip packages with the independently versioned `0.0.1` XZ packages. They combine payload changes and compression changes; the download reduction is not solely code removal. These are historical candidate measurements, not current npm download sizes. MB means 1,000,000 bytes; unpacked size sums regular files and excludes filesystem allocation and dependency packages.

| Engine | Previous download | 0.0.1 download | Reduction | Previous unpacked | 0.0.1 unpacked |
| --- | ---: | ---: | ---: | ---: | ---: |
| macOS ARM64 | 98.85 MB | 60.49 MB | 38.81% | 301.04 MB | 269.43 MB |
| Node WASM | 56.47 MB | 35.85 MB | 36.51% | 210.24 MB | 190.64 MB |

The recorded local macOS ARM64 check installed the same candidate offline with native and WASM selection. Six synthetic DOCX/XLSX/PPTX documents per engine, covering Chinese/English text, tables, formulas, and images, retained identical extracted text, page counts, and 96-DPI rendered pixels against the preceding packages. This covers those fixtures and that host; it does not establish universal document fidelity or other-platform qualification. Runtime suites also cover external-link suppression, font substitutions, limits, and cancellation; [release qualification](docs/building.md) requires installed-engine evidence.

## Build guide

See [Building and qualifying engines](docs/building.md) for platform prerequisites,
caller-selected paths, explicit parallelism, and package qualification. Use the
source and toolchain versions pinned in the checkout being built.

## Development

[Native sources](engine/native/) and the [Node WASM recipe](engine/wasm-source/README.md) use the same LibreOffice revision pinned by the `engine/core` submodule. [.gitmodules](.gitmodules) owns its upstream URL and the gitlink owns its commit. Checkout scripts keep that submodule pristine and create disposable, patchable trees under ignored `.build/` directories. This repository owns component selection and the complete packaging recipe; each engine ships the matching source and license materials.

Run the repository checks without building LibreOffice:

```sh
pnpm verify:metadata
pnpm test
pnpm test:packaging
```

Real engine tests additionally require `pnpm run build:adapter`, prepared engine payloads, and `LIBREOFFICE_RUNTIME_ENTRY` pointing to the absolute path of `packages/entry/lib/index.js`. Build platforms sequentially with explicit parallelism suited to CPU and memory; `pnpm gha:matrix` shows the CI matrix. Build output remains ignored.

[Benchmarks](benchmarks/README.md) measure the public disk API in separate native and WASM installations and distinguish first font indexing from converter reuse. Use the same source revision, documents, font roots, and PDF options for comparisons; frontend transport and painting are separate measurements.

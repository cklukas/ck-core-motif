# Hardening baseline and audit ledger

CK Core Motif is a legacy C toolkit that is being audited for null-pointer,
memory, overflow and unhandled-edge-case defects and brought to C23. This page
records the measured starting point and which parts have been audited, so each
change can be checked against it. The opt-in build switches are described in
[Build and install](build.md#hardening-and-sanitizer-builds).

## Method

Configure with `--enable-hardening`, build at `-O2` and keep the complete `make`
log. The counting tool `scripts/warning-baseline.py` attributes each compiler
warning to a component, counts it once however many translation units include
its header, and compares a later log with a recorded baseline:

```sh
scripts/warning-baseline.py record make.log --depth 2 --strip "$srcdir" --strip "$builddir" --label motif-hard > baseline.json
scripts/warning-baseline.py compare make.log baseline.json --depth 2 --strip "$srcdir" --strip "$builddir"
```

`compare` fails when any component and class has more warnings than the
baseline, so the baseline is a ratchet: counts may only fall. The `list` mode
prints the individual warnings of chosen classes for triage. Warnings are not
promoted to errors; a class becomes `-Werror` for a component once its count
reaches zero.

## Baseline 2026-11-04

GCC 14.2 on Debian 13 arm64, `-std=c23 -O2 -g0`, `--enable-hardening`, six jobs,
with Xft, Unicode text, AT-SPI and SVG enabled. The build of the libraries,
WML, UIL and mwm took 46 seconds. The
machine-readable record is
[hardening-baseline-validation.json](hardening-baseline-validation.json).

The build reports 1,938 unique warnings.
242 of them belong to classes that
point at memory-safety, lifetime or error-handling defects; the rest are style
and signedness classes, mainly `-Wmissing-field-initializers` and
`-Wsign-compare`.

| Warning class | Unique warnings |
|---|---:|
| `-Wformat-security` | 75 |
| `-Wimplicit-fallthrough` | 49 |
| `-Wnull-dereference` | 42 |
| `-Wmisleading-indentation` | 32 |
| `-Wcast-function-type` | 30 |
| `-Wpointer-to-int-cast` | 7 |
| `-Wmaybe-uninitialized` | 3 |
| `-Waddress` | 2 |
| `-Wuninitialized` | 1 |
| `-Wint-to-pointer-cast` | 1 |

cppcheck reports 329 messages in product code (62 errors, 255 warnings, 12 portability). The error identifiers are `uninitvar` 52, `memleak` 5, `legacyUninitvar` 2, `arrayIndexOutOfBounds` 1, `shiftNegative` 1, `memleakOnRealloc` 1. Test and demo directories are excluded, as are cppcheck's
own analysis-depth notices, parse failures and `va_list_usedBeforeStarted`
reports, which come from cppcheck not knowing the toolkit's `Va_start` macro.

## Audit ledger

A component is *audited* when a topic commit records a review of its complete
input path, not when a compiler is quiet. The commit activity below is a weaker
signal: it shows which files have been touched at all since 2026-10-22. Fuzz
targets exist for none of the components yet.

| Component | Exposure | Prior hardening | Unique warnings | Defect-class | cppcheck errors | Fuzz targets |
|---|---|---|---:|---:|---:|---|
| `lib/Xm` | toolkit library; clipboard, selection, text, string and image paths; editres protocol | hardening commits touch most files since 2026-10-22 | 907 | 136 | 27 | none |
| `clients/uil` | UIL compiler; parses source files | none recorded | 540 | 15 | 2 | none |
| `clients/mwm` | window manager; resource and property parsers | control flow, initialization and keysym handling (2026-11-01) | 197 | 13 | 27 | none |
| `lib/Mrm` | reads binary UID files | hardening commits since 2026-10-22 | 185 | 10 | 0 | none |
| `tools/wml` | widget meta-language generator; build-time tool | none recorded | 103 | 68 | 5 | none |
| `config/util` | build-time utility | none recorded | 6 | 0 | 1 | none |

Source files with no commit of any kind since 2026-10-22 (first-party C, yacc
and lex sources):

- `lib/Xm` (22): AtomMgr, CvtUnit, Dest, DialogSE, DrHiDash, DrPoly, FocusAct,
  GetRes, IconH, MenuProc, Messages, ObsoStr, ObsoXme, ReadImage, ShellE,
  SlideC, TearOffB, TranslNew, Transltns, TravAct, ValTime, sharedlib.
- `lib/Mrm` (4): MrmMessages, Mrmlwrite, Mrmos, Mrmwwrite.
- `clients/mwm` (2): WmCPlace, version. `clients/uil` (1): UilData.
  `tools/wml` (1): Uil.y.

The vendored libXpm sources (31 files in `lib/Xm`, 7 `.c` files unchanged) are
reviewed but not mass-rewritten, as are the libXmu-derived parts of
`lib/Xm/EditresCom.c`. The optional features added in October and November 2026
(appearance profiles, AT-SPI accessibility, text layout, PNG, SVG and JPEG
loading, XLibre scaling) are the newest and least exercised code; they have had
the least time in real use and are priorities for the same review.

## Known limits

- The sanitized build compiles and links the libraries, generators and
  programs. The installed-prefix fixtures have not yet been run under the
  sanitizers; the baseline covers compilation and static analysis only.
- cppcheck ran with `--max-configs=1`, so code behind alternative `#ifdef`
  configurations is not analysed.
- `tests/automated` holds thousands of source-text guards that assert exact
  source text. A mechanical rewrite must update the affected guards in the same
  change.

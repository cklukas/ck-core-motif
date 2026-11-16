# Hardening baseline and audit ledger

CK Core Motif is a legacy C toolkit that is being audited for null-pointer,
memory, overflow and unhandled-edge-case defects and brought to C23. This page
records the measured starting point, the current WP-42 defect-warning ratchet,
and which parts have been audited, so each change can be checked against it.
The opt-in build switches are described in
[Build and install](build.md#hardening-and-sanitizer-builds).

## Method

Configure with `--enable-hardening`, build at `-O2` and keep the complete `make`
log. The counting tool `scripts/warning-baseline.py` attributes each compiler
warning to a component, counts it once however many translation units include
its header, and compares a later log with a recorded baseline:

```sh
scripts/warning-baseline.py record make.log --depth 2 --strip "$srcdir" --strip "$builddir" --label motif-hard > baseline.json
scripts/warning-baseline.py compare make.log docs/hardening-baseline-validation.json --depth 2 --strip "$srcdir" --strip "$builddir"
```

`compare` fails when any component and class has more warnings than the
baseline, so the baseline is a ratchet: counts may only fall. The `list` mode
prints the individual warnings of chosen classes for triage. Once a defect-oriented
warning class reaches zero across all components, it is promoted to `-Werror=...`
under `--enable-hardening` in `ck_hardening.m4` and locked at zero in
[hardening-baseline-validation.json](hardening-baseline-validation.json).

## Baseline 2026-11-15 (WP-42 ratchet)

GCC 14.2 and Clang 19 on Debian 13 arm64, `-std=c23 -O2 -g0`, `--enable-hardening`,
six jobs, with Xft, Unicode text, AT-SPI and SVG enabled. The build of the
libraries, WML, UIL and mwm took 21 seconds. The machine-readable record is
[hardening-baseline-validation.json](hardening-baseline-validation.json), and
the ratchet is verified by `tests/automated/check-hardening-defect-ratchet.py`.

The initial 2026-11-04 baseline reported 1,938 unique warnings (242 in the 10
defect-oriented classes) and 329 `cppcheck` messages in product code (62 errors:
`uninitvar` 52, `memleak` 5, `legacyUninitvar` 2, `arrayIndexOutOfBounds` 1,
`shiftNegative` 1, `memleakOnRealloc` 1). Following WP-42, all 10 defect-oriented
warning classes and all 62 product-code `cppcheck` errors have been reduced to
zero; the remaining 1,653 unique compiler warnings are style and signedness
classes (mainly `-Wmissing-field-initializers` and `-Wsign-compare`), and
`cppcheck` reports 105 non-error messages in product code (0 errors, 93 warnings,
12 portability).

| Warning class | 2026-11-04 baseline | 2026-11-15 (WP-42) | Hardening status |
|---|---:|---:|---|
| `-Wformat-security` | 75 | 0 | `-Werror=format-security` |
| `-Wimplicit-fallthrough` | 49 | 0 | `-Werror=implicit-fallthrough=3` |
| `-Wnull-dereference` | 42 | 0 | `-Werror=null-dereference` |
| `-Wmisleading-indentation` | 32 | 0 | `-Werror=misleading-indentation` |
| `-Wcast-function-type` | 30 | 0 | `-Werror=cast-function-type` |
| `-Wpointer-to-int-cast` | 7 | 0 | `-Werror=pointer-to-int-cast` |
| `-Wmaybe-uninitialized` | 3 | 0 | `-Werror=maybe-uninitialized` |
| `-Waddress` | 2 | 0 | `-Werror=address` |
| `-Wuninitialized` | 1 | 0 | `-Werror=uninitialized` |
| `-Wint-to-pointer-cast` | 1 | 0 | `-Werror=int-to-pointer-cast` |

Test and demo directories are excluded from the `cppcheck` product-code ledger,
as are `cppcheck`'s own analysis-depth notices, parse failures and
`va_list_usedBeforeStarted` reports, which come from `cppcheck` not knowing the
toolkit's `Va_start` macro.

## Audit ledger

A component is *audited* when a topic commit records a review of its complete
input path, not when a compiler is quiet. The commit activity below is a weaker
signal: it shows which files have been touched at all since 2026-10-22. Fuzz
targets exist for `lib/Mrm` (`fuzz-mrm-uid` and the reproducible damaged-UID
test, [UID files as untrusted input](mrm-uid-input.md)) and for `lib/Xm`
(`fuzz-editres-wire`, `fuzz-xm-converters`, `fuzz-xmstring-stream`; see
[Fuzzing and corpus replay](fuzzing.md)); the Editres handler and the
D-Bus adapter also have compiled behavioural tests under the sanitizers
([Editres protocol handler](editres-wire-protocol.md),
[Accessibility D-Bus reply building](accessibility-dbus-reply-building.md)).

| Component | Exposure | Prior hardening | Unique warnings | Defect-class | cppcheck errors | Fuzz targets |
|---|---|---|---:|---:|---:|---|
| `lib/Xm` | toolkit library; clipboard, selection, text, string and image paths; editres protocol | hardening commits since 2026-10-22; WP-42 defect-warning & cppcheck ratchet and WP-43 legacy/modern subsystem audit ([WP-43 audit](untrusted-input-audit-wp43.md), 2026-11-16) | 745 | 0 | 0 | `fuzz-editres-wire`, `fuzz-xm-converters`, `fuzz-xmstring-stream` |
| `clients/uil` | UIL compiler; parses source and `.wmd` files | WP-42 defect-warning & cppcheck ratchet; WP-43 WMD binary database and parser bounds audit ([WP-43 audit](untrusted-input-audit-wp43.md), 2026-11-16) | 525 | 0 | 0 | none |
| `clients/mwm` | window manager; resource and property parsers | control flow, initialization and keysym handling (2026-11-01); WP-42 ratchet and WP-43 interactive-placement bounds audit (2026-11-16) | 173 | 0 | 0 | none |
| `lib/Mrm` | reads and writes binary UID files | hardening commits since 2026-10-22; UID index, literal, icon and compression-table checks (2026-11-10); WP-42 ratchet and WP-43 writer/OS audit (2026-11-16) | 173 | 0 | 0 | `fuzz-mrm-uid`, `run-mrm-uid-mutations.py` |
| `tools/wml` | widget meta-language generator; build-time tool | WP-42 format-security & cppcheck ratchet; WP-43 WMD emitter/grammar review (2026-11-16) | 31 | 0 | 0 | none |
| `config/util` | build-time utility | WP-42 checktree directory-handle leak fix (2026-11-15) | 6 | 0 | 0 | none |

All 28 previously untouched first-party C/yacc files from the 2026-11-04
ledger have been audited in [WP-43](untrusted-input-audit-wp43.md):
executable modules (`AtomMgr`, `CvtUnit`, `Dest`, `DialogSE`, `DrHiDash`,
`DrPoly`, `FocusAct`, `GetRes`, `IconH`, `MenuProc`, `ObsoXme`, `ReadImage`,
`ShellE`, `SlideC`, `TearOffB`, `TravAct`, `ValTime`, `Mrmlwrite`, `Mrmos`,
`Mrmwwrite`, `WmCPlace`) received explicit input/bounds/lifetime guards, and
pure constant/table translation units (`Messages`, `ObsoStr`, `TranslNew`,
`Transltns`, `sharedlib`, `MrmMessages`, `version`, `UilData`, `Uil.y`) were
verified free of mutable untrusted-input paths.

The vendored libXpm sources (31 files in `lib/Xm`, 7 `.c` files unchanged) are
reviewed but not mass-rewritten, as are the libXmu-derived parts of
`lib/Xm/EditresCom.c`. The optional features added in October and November 2026
(appearance profiles, AT-SPI accessibility, text layout, PNG, SVG and JPEG
loading, XLibre scaling) were audited in
[WP-43](untrusted-input-audit-wp43.md) for dimension/channel overflow,
scanline buffer bounds, Xlib region/GC allocation checks, and reentrant
teardown.

## Known limits

- The sanitized build compiles and links the libraries, generators and
  programs. The installed-prefix fixtures have not yet been run under the
  sanitizers; the baseline covers compilation and static analysis only.
- cppcheck ran with `--max-configs=1` for the baseline summary, though the
  product-code directories (`lib/Xm`, `lib/Mrm`, `clients/mwm`, `clients/uil`,
  `tools/wml`, `config/util`) were also verified at default multi-branch depth
  with zero `cppcheck` errors.
- `tests/automated` holds thousands of source-text guards that assert exact
  source text. A mechanical rewrite must update the affected guards in the same
  change.

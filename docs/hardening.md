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

## Baseline 2026-11-27 (Phase 3 warning ratchet)

GCC 15.2 and Clang 21.1 on Ubuntu 26.04.1 LTS arm64, `-std=c23 -O2 -g0`,
`--enable-hardening`, six jobs, with Xft, Unicode text, AT-SPI, SVG, XLibre DPI,
and native Wayland enabled. The clean build of the libraries, WML, UIL, and `mwm`
completed with zero errors under both GCC and Clang (and zero warnings under Clang).
The machine-readable record
is [hardening-baseline-validation.json](hardening-baseline-validation.json), and
the ratchet is verified by `tests/automated/check-hardening-defect-ratchet.py`.

The initial 2026-11-04 baseline reported 1,938 unique warnings (242 in the 10
defect-oriented classes) and 329 `cppcheck` messages in product code (62 errors:
`uninitvar` 52, `memleak` 5, `legacyUninitvar` 2, `arrayIndexOutOfBounds` 1,
`shiftNegative` 1, `memleakOnRealloc` 1). Following WP-42, Phase 2 (2026-11-26),
and Phase 3 of the warning ratchet (2026-11-27), all 10 original defect-oriented
warning classes, all 62 product-code `cppcheck` errors, `-Wmissing-field-initializers`
(671), `-Wsign-compare` (485), `-Wformat-nonliteral` (135), `-Wunused-variable`
(107), `-Wempty-body` (64), `-Wunused-but-set-variable` (52), `-Wunused-function`
(16), `-Wunused-label` (2), and `-Wunused-local-typedefs` (1) across `lib/Xm`,
`lib/Mrm`, `clients/mwm`, `clients/uil`, `tools/wml`, and `config/util` have been
reduced to zero and promoted to `-Werror=...` under `--enable-hardening` (19
promoted `-Werror=...` classes total); `clients/uil`, `tools/wml`, and
`config/util` build with zero warnings, and the only remaining 125 GCC diagnostics
are intentional 32-bit/wide-prototype `SIZE_MAX` / `RGMListSizeMax` range guards
(`-Wtype-limits` on 64-bit targets, 0 under Clang), while `cppcheck` reports 105
non-error messages in product code (0 errors, 93 warnings, 12 portability).

| Warning class | 2026-11-04 baseline | 2026-11-15 (WP-42) | 2026-11-26 (Phase 2) | 2026-11-27 (Phase 3) | Hardening status |
|---|---:|---:|---:|---:|---|
| `-Wmissing-field-initializers` | 671 | 671 | 0 | 0 | `-Werror=missing-field-initializers` |
| `-Wsign-compare` | 485 | 485 | 0 | 0 | `-Werror=sign-compare` |
| `-Wformat-nonliteral` | 135 | 135 | 135 | 0 | `-Werror=format-nonliteral` |
| `-Wunused-variable` | 107 | 107 | 107 | 0 | `-Werror=unused-variable` |
| `-Wformat-security` | 75 | 0 | 0 | 0 | `-Werror=format-security` |
| `-Wempty-body` | 64 | 64 | 64 | 0 | `-Werror=empty-body` |
| `-Wunused-but-set-variable` | 52 | 52 | 52 | 0 | `-Werror=unused-but-set-variable` |
| `-Wimplicit-fallthrough` | 49 | 0 | 0 | 0 | `-Werror=implicit-fallthrough=3` |
| `-Wnull-dereference` | 42 | 0 | 0 | 0 | `-Werror=null-dereference` |
| `-Wmisleading-indentation` | 32 | 0 | 0 | 0 | `-Werror=misleading-indentation` |
| `-Wcast-function-type` | 30 | 0 | 0 | 0 | `-Werror=cast-function-type` |
| `-Wunused-function` | 16 | 16 | 16 | 0 | `-Werror=unused-function` |
| `-Wpointer-to-int-cast` | 7 | 0 | 0 | 0 | `-Werror=pointer-to-int-cast` |
| `-Wmaybe-uninitialized` | 3 | 0 | 0 | 0 | `-Werror=maybe-uninitialized` |
| `-Waddress` | 2 | 0 | 0 | 0 | `-Werror=address` |
| `-Wunused-label` | 2 | 2 | 2 | 0 | `-Werror=unused-label` |
| `-Wuninitialized` | 1 | 0 | 0 | 0 | `-Werror=uninitialized` |
| `-Wint-to-pointer-cast` | 1 | 0 | 0 | 0 | `-Werror=int-to-pointer-cast` |
| `-Wunused-local-typedefs` | 1 | 1 | 1 | 0 | `-Werror=unused-local-typedefs` |

Test and demo directories are excluded from the `cppcheck` product-code ledger,
as are `cppcheck`'s own analysis-depth notices, parse failures and
`va_list_usedBeforeStarted` reports, which come from `cppcheck` not knowing the
toolkit's `Va_start` macro.

## Audit ledger

A component is *audited* when a topic commit records a review of its complete
input path, not when a compiler is quiet. The commit activity below is a weaker
signal: it shows which files have been touched at all since 2026-10-22. Fuzz
targets exist for `lib/Mrm` (`fuzz-mrm-uid` and the reproducible damaged-UID
test, [UID files as untrusted input](mrm-uid-input.md)), for `clients/uil`
(`fuzz-uil-grammar`, `fuzz-uil-wmd`), for `clients/mwm` (`fuzz-mwm-parsers`),
and for `lib/Xm` (`fuzz-editres-wire`, `fuzz-image-cache`, `fuzz-wayland-x11-server`,
`fuzz-xm-converters`, `fuzz-xmstring-ct`, `fuzz-xmstring-stream`,
`fuzz-xpm-image`; see [Known limits](#known-limits)); the Editres handler and the
D-Bus adapter also have compiled behavioural tests under the sanitizers
([Editres protocol handler](editres-wire-protocol.md),
[Accessibility D-Bus reply building](accessibility-dbus-reply-building.md)).

| Component | Exposure | Prior hardening | Unique warnings | Defect-class | cppcheck errors | Fuzz targets |
|---|---|---|---:|---:|---:|---|
| `lib/Xm` | toolkit library; clipboard, selection, text, string and image paths; editres protocol; native Wayland X11/XRender server | hardening commits since 2026-10-22; WP-42 defect-warning & cppcheck ratchet, WP-43 legacy/modern subsystem audit ([WP-43 audit](untrusted-input-audit-wp43.md)), WP-44 XPM fuzzer & bounds fixes (2026-11-16), Wayland backend ASan/UBSan/TSan + wire fuzzer qualification, Phase 2 `-Wsign-compare` / `-Wmissing-field-initializers` ratchet (2026-11-26), ISO-2022 Compound Text / `ImageCache` fuzzing & multibyte bounds fixes, and Phase 3 warning ratchet (2026-11-27) | 90 | 0 | 0 | `fuzz-editres-wire`, `fuzz-image-cache`, `fuzz-wayland-x11-server`, `fuzz-xm-converters`, `fuzz-xmstring-ct`, `fuzz-xmstring-stream`, `fuzz-xpm-image` |
| `clients/uil` | UIL compiler; parses source and `.wmd` files | WP-42 defect-warning & cppcheck ratchet; WP-43 WMD binary database and parser bounds audit ([WP-43 audit](untrusted-input-audit-wp43.md)); WP-44 `.wmd` fuzzer (2026-11-16); Phase 2 initializer & signedness ratchet (2026-11-26); callable `Uil()` grammar/UID fuzzer, lifetime/bounds fixes, and Phase 3 zero-warning ratchet (2026-11-27) | 0 | 0 | 0 | `fuzz-uil-grammar`, `fuzz-uil-wmd`, `check-wmd.py` |
| `clients/mwm` | window manager; resource and property parsers | control flow, initialization and keysym handling (2026-11-01); WP-42 ratchet, WP-43 interactive-placement bounds audit, WP-44 WSM wire & `.mwmrc` fuzzer (2026-11-16), Phase 2 signedness ratchet (2026-11-26), and Phase 3 format/unused ratchet (2026-11-27) | 7 | 0 | 0 | `fuzz-mwm-parsers`, `wsm-wire-validation` |
| `lib/Mrm` | reads and writes binary UID files | hardening commits since 2026-10-22; UID index, literal, icon and compression-table checks (2026-11-10); WP-43 writer/OS audit, WP-44 `-O1` sanitizer initialization fix (2026-11-16), Phase 2 signedness ratchet (2026-11-26), and Phase 3 `__format_arg__` + 64-bit `snprintf` type fixes (2026-11-27) | 28 | 0 | 0 | `fuzz-mrm-uid`, `run-mrm-uid-mutations.py` |
| `tools/wml` | widget meta-language generator; build-time tool | WP-42 format-security & cppcheck ratchet; WP-43 WMD emitter/grammar review; WP-44 sanitized `wmldbcreate` round-trip (2026-11-16); Phase 2 initializer ratchet (2026-11-26); Phase 3 zero-warning ratchet (2026-11-27) | 0 | 0 | 0 | `check-wmd.py` |
| `config/util` | build-time utility | WP-42 checktree directory-handle leak fix (2026-11-15); Phase 3 zero-warning ratchet (2026-11-27) | 0 | 0 | 0 | none |

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

- The sanitized (`--enable-sanitizers=address,undefined` and
  `--enable-sanitizers=thread`) installed-prefix builds, `run-wayland-backend.py`,
  and all 11 `libFuzzer` replay targets (`editres-wire`, `image-cache`,
  `mrm-uid`, `mwm-parsers`, `uil-grammar`, `uil-wmd`, `wayland-x11-server`,
  `xm-converters`, `xmstring-ct`, `xmstring-stream`, `xpm-image`),
  `check-wmd.py`, `check-xpm-input`, `wsm-wire-validation`, and the 5 sanitized
  parser-bounds suites (`run-xmstring-stream-bounds.py`,
  `run-resconvert-list-bounds.py`, `run-mrm-literal-bounds.py`,
  `run-mrm-icon-bounds.py`, `run-mrm-value-bounds.py`) pass with zero
  toolkit-owned ASan/UBSan diagnostics. Leak-enabled process-exit allocations
  inside external system libraries (`libX11`, `libXt`, `fontconfig`) remain
  documented external ownership boundaries in `docs/status.md`.
- cppcheck ran with `--max-configs=1` for the baseline summary, though the
  product-code directories (`lib/Xm`, `lib/Mrm`, `clients/mwm`, `clients/uil`,
  `tools/wml`, `config/util`) were also verified at default multi-branch depth
  with zero `cppcheck` errors.
- `tests/automated` holds thousands of source-text guards that assert exact
  source text. A mechanical rewrite must update the affected guards in the same
  change.

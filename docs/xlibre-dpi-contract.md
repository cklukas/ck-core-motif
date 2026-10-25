---
nav_exclude: true
---
# XLibre per-output DPI contract

The optional `--enable-xlibre-dpi` build enables a narrow probe for the XLibre
RandR output property named `DPI`. This is a scale hint, not a change to the
X11 coordinate system. XLibre defines 96 as the unscaled baseline; 144 and
192 therefore represent 1.5x and 2x hints.

The probe accepts a value only when all of these conditions hold:

- the server vendor string is exactly `XLibre` or starts with `XLibre` followed
  by whitespace (normally the server version);
- RandR is present at version 1.2 or newer;
- the output property has type `INTEGER`, format 32, exactly one item and no
  trailing data; and
- the signed value is in the documented range 1 through 4096 DPI.

Missing, malformed, out-of-range and non-XLibre values are unsupported and do
not become a zero or guessed scale. The optional build is disabled by default;
requesting it without the Xrandr development header and library fails during
configuration rather than producing a partially working library.

This package only validates and reads the property. It intentionally does not
yet select an output for a shell, subscribe to RandR events or reflow realized
widgets. Those operations require a separate UI-thread transaction contract.
The installed fixture checks the parser's valid and invalid cases and verifies
that ordinary Xvfb falls back safely. Pass `--display` to the runner when a
real XLibre display is available; in that mode every reported output must
expose a valid `DPI` property.
If a realized shell's client-area or RandR resource query is temporarily
unavailable, the observer returns the existing global display scale and output
`None`. It does not retain a stale per-output scale across a transient query
failure. Output removal with valid RandR resources follows the same fallback
through the no-selected-output path.

The vendor check is token-based rather than a substring search: names such as
`NotXLibre` and `XLibre-server` are rejected. This keeps the optional adapter
bound to the server family that defines the property instead of treating an
unrelated implementation's name as evidence of the contract.

The property decoder validates that metadata before reading the Xlib buffer.
This ordering matters on 64-bit hosts: a malformed format-8 or format-16
reply can be shorter than the `long` representation Xlib uses for format 32.
The installed contract fixture passes a null buffer for those malformed shapes
and verifies rejection under the sanitizer configuration, so the safety
guarantee does not depend only on the final Boolean result.

The internal shell-boundary policy is now defined without changing the public
Widget ABI. For a realized shell client area, the output containing its center
is selected. If the center is in no output (for example while a window is in a
gap), the nearest output rectangle is selected using the sum of horizontal and
vertical distances; equal distances use the lowest RandR output id. Empty or
invalid geometry returns `None`, allowing the caller to retain the existing
global-DPI fallback. All descendants of a shell will therefore have one
selected scale rather than mixing output scales.

The installed fixture checks the parser, ordinary-Xvfb fallback and this
selection policy. Pass `--display` to the runner when a real XLibre display is
available; in that mode every reported output must expose a valid `DPI`
property. Each successful run writes `receipt.json` beside its logs with the
source hashes, exact compile/run commands, display mode, elapsed time and
installed-library fingerprint. The live shell integration below is separately
gated because it requires a real multi-output XLibre session.

## Live shell integration (opt-in build)

When `--enable-xlibre-dpi` is enabled, a realized VendorShell is registered
with the owning Xt application context. The toolkit selects the RandR
notifications on the screen root, drains only those notifications from the
Xlib queue on the UI thread, and schedules one Xt work procedure for a burst
of changes. The work procedure resolves the shell's current output and scale,
applies the shared UI-thread scale transaction to that shell's descendant
hierarchy, and then calls the internal change hook after the resize is stable.
Explicit pixel resources are never rewritten; registered controls update only
documented derived/default metrics. Shell destruction removes the input and
pending work procedure before releasing the state.

The installed observer fixture runs on the requested `--display` when a real
XLibre server is supplied. It checks registration for two realized shells on
one Xt display, positive shell and descendant scale lookup, idempotent
re-registration, explicit unregistration before shell destruction, retirement
of one shell without invalidating the other, and refusal before realization.
Explicit unregistration removes the destroy callback before releasing the
state, so a later shell teardown cannot call through a stale closure. Without
`--display` it runs under Xvfb and requires the ordinary-server fallback. This
keeps the fallback test from being mistaken for live XLibre qualification.

The queued observer work procedure retains its state while it runs. This is
required because the normal Xt resize transaction can dispatch application
callbacks, and one of those callbacks may destroy the shell and retire the
observer before the work procedure has returned. Retirement unlinks the state
and cancels future input/work notifications, but reclamation waits for the
active work reference. The work procedure also skips the optional change hook
after reentrant destruction. `check-xlibre-lifetime.py` keeps these ordering
invariants in the automated source-contract checks; the live multi-output
destruction case still requires an XLibre display.

On 2026-10-25, a fresh x86_64 adapter-enabled prefix compiled this lifetime
change and passed the parser/fallback, deterministic output-selection and
ordinary-server observer fixtures under Xvfb. The installed
`libXm.so.8.0.0` fingerprint was
`58bc39a032d63a600942e90554ab67655dd08eed4b3303e878b59e3f135ccf94`; the
task-local runner output is retained at
`/mnt/v/tmp/ckmotif-x86-20261025/results/xlibre-r2/`. No real XLibre
multi-output server was available, so live property mutation, output
hotplug, spanning-window reflow and destruction during queued reflow remain
unqualified.

The decode-hardening follow-up was rebuilt on AArch64 `uconsole` from
13:59:58 through 14:03:17 UTC on 2026-10-25 (clean opt-in build, 3:03
elapsed), installing to `/tmp/ckmotif-xlibre-hardening-20261025/prefix`.
Three fresh ordinary-Xvfb contract runs and a consumer ASan/UBSan run
(`ASAN_OPTIONS=detect_leaks=0`, `UBSAN_OPTIONS=halt_on_error=1`) passed. The
installed `libXm.so.8.0.0` SHA-256 was
`21d18b98341991c6e0e04ce3d56121c1c2f76c7b3e4b9e3809e39a53c52f6a59`; logs
remain under `/tmp/ckmotif-xlibre-hardening-20261025/`. This is malformed
property safety evidence only and does not qualify a real multi-output
XLibre server.

The vendor-token follow-up was an incremental rebuild ending at 14:05:29 UTC
on the same host and prefix. Three fresh installed contract runs and the
consumer ASan/UBSan decoder run passed; the resulting `libXm.so.8.0.0`
fingerprint is
`e8e0655256f43b187ea45c06e78f603976835bd1207959ae8ee9da122a6150f1`.
The separate `vendor-v1` through `vendor-v3` and `sanitizer-vendor` receipts
remain under `/tmp/ckmotif-xlibre-hardening-20261025/`.

The transient-query fallback rebuild completed at 14:06:32 UTC on the same
prefix. Three fresh installed contract runs, the source-contract guard and a
consumer ASan/UBSan decoder run passed; the installed library fingerprint is
`dc2ff61fd9994fcb261a861fd702657c80f2add301ccdee7cb13f7e764df6144`.
The `fallback-f1` through `fallback-f3` and `sanitizer-fallback` receipts are
retained under `/tmp/ckmotif-xlibre-hardening-20261025/`.
The receipt-schema validation run is retained at
`/tmp/ckmotif-xlibre-hardening-20261025/receipt-r1/receipt.json` and records
the same installed fingerprint.

This is an implementation step toward the live contract, not a claim that a
generic Xorg server provides per-output DPI. The remaining qualification must
exercise property changes, output removal/addition, a spanning window and
destruction during a queued update on a real multi-output XLibre session.

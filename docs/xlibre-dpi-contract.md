# XLibre per-output DPI contract

The optional `--enable-xlibre-dpi` build enables a narrow probe for the XLibre
RandR output property named `DPI`. This is a scale hint, not a change to the
X11 coordinate system. XLibre defines 96 as the unscaled baseline; 144 and
192 therefore represent 1.5x and 2x hints.

The probe accepts a value only when all of these conditions hold:

- the server identifies itself as XLibre;
- RandR is present at version 1.2 or newer;
- the output property has type `INTEGER`, format 32, exactly one item and no
  trailing data; and
- the signed value is in the documented range 1 through 4096 DPI.

Missing, malformed, out-of-range and non-XLibre values are unsupported and do
not become a zero or guessed scale. The optional build is disabled by default;
requesting it without the Xrandr development header and library fails during
configuration rather than producing a partially working library.

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
property. The live shell integration below is separately gated because it
requires a real multi-output XLibre session.

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

This is an implementation step toward the live contract, not a claim that a
generic Xorg server provides per-output DPI. The remaining qualification must
exercise property changes, output removal/addition, a spanning window and
destruction during a queued update on a real multi-output XLibre session.

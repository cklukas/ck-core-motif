# Periodic Table as a toolkit appearance reference

`demos/programs/periodic` now starts with toolkit modern-light defaults. Use
`--appearance modern-dark`, `--appearance high-contrast`, `--appearance classic`
or `--density compact` to compare the same UIL hierarchy. Its modern resource
class is `XmdPeriodicModern`; it supplies no fallback font, color, border or
control-size overrides. The UIL retains ownership of content and arrangement.

`--historical-resources` instead selects classic appearance, the original
fallback resources and application class `XmdPeriodic`. It can load the supplied
`XmdPeriodic.ad` file from an app-defaults directory. Combining this mode with a
non-classic appearance is an error. Historical resources should not be merged
into a global wildcard database when comparing toolkit defaults.

Appearance and density are selected after opening the display and before creating
the session shell: shell initialization itself can create internal Motif objects.
All fetched controls, menus and dialogs then observe the selected settings.
Standard Xt options and explicit resources remain authoritative. Missing UID
files and failed hierarchy construction are reported with a failing exit status.
The realized shell publishes `_NET_WM_PID` for actual-process visual verification.

## Toolkit defects exposed by this reference

Static labels and label gadgets consistently opt out of keyboard traversal in
modern light and dark. A title is not an actionable control; interactive
subclasses retain their normal traversal and applications may opt labels in.

Ordinary and option RowColumns do not accept nonzero shadows. Profile resolution
now preserves their class baseline instead of repeatedly asking SetValues to
reject a theme shadow. Popup, pulldown and menu-bar shadows remain styled.

Classic Periodic startup exposed a List geometry loop with no progress for
zero-height rows and zero spacing. A Dimension-sized accumulator could also
wrap at large available heights. Query and resize now share measured row-height
resolution and a widened quotient. Rows occupy at least one pixel, including
empty lists without an available default font; at least one row is reported for
an undersized viewport. No iteration proportional to the viewport is required.

The actual light/dark windows exposed missing labels even when startup/resource
checks passed. A tag-cache input guard rejected `XmSTRING_TAG_STRLEN` (`-1`),
which is a valid sentinel, so a fresh cache returned null font tags. The guard
now distinguishes that sentinel from invalid negative lengths. Native coverage
starts with an empty tag cache, checks sentinel/bounded tag equivalence, and
requires positive measured extents for a real default-font label. UIL resources
must be regenerated with the corrected library; both UIL input files are now
explicit build dependencies.

## Native validation

Run `tests/automated/run-periodic-appearance.py --build /absolute/build --out
/absolute/fresh-output` on Linux after building Xm, Mrm, UIL and Periodic. The
fixture compiles the actual demo startup and substitutes only its event loop to
inspect the realized hierarchy. It checks light, dark, high contrast, compact,
classic, historical resources and explicit color overrides, including menu and
dialog resources. A historical app-defaults file is installed for the test to
verify class isolation. Zero, one and maximum row heights are queried with a
maximum viewport; startup errors are tested separately. Receipts record source
and loaded library hashes. Xvfb readiness is obtained through its display pipe.

This startup/resource acceptance does not establish visual-overhaul completion.
Default typography, semantic roles, control geometry, redesigned controls and the
full actual-window state/DPI matrix remain in `modern-visual-overhaul.md`.

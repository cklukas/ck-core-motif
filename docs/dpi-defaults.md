---
nav_exclude: true
---
# Global DPI defaults

ck-core-motif reads the optional `Xft.dpi` X resource when a widget needs a
scaled default. The value is accepted only when it is finite, positive and
fully consumed after surrounding whitespace. The scale is `Xft.dpi / 96`. One logical pixel therefore maps to one device
pixel at 96 DPI, consistently with the widget-scale override and XLibre scale.
The ABI 9 development cohort replaces the former 75-DPI conversion; applications
which used that conversion when constructing overrides should now pass their
intended logical scale directly.

Only toolkit-owned defaults are scaled. Explicit application resources remain
authoritative: an explicit `XmNarrowSize` or `XmNindicatorSize` is returned
unchanged at every DPI. Invalid values fall back to the existing unscaled
default, and an oversized finite result saturates at the maximum `Dimension`
instead of wrapping during conversion.

Native test tools may use `XmSetDisplayScaleOverride` from `<Xm/Display.h>` to
exercise the same default conversion deterministically.  The override is
per-display and process-local, does not mutate `Xft.dpi` or the X resource
database, and is cleared with `XmClearDisplayScaleOverride`.  Changing the
value runs the shared UI-thread scale transaction: registered scale-aware
widgets update toolkit-owned defaults, then the normal same-size resize pass
reflows realized geometry.  Widget identity, callbacks, toggle state, and
explicit pixel resources are preserved.

The current qualified scope is deliberately global-per-display. It covers
unlabelled ToggleButton and ToggleButtonGadget indicators and SpinBox arrow
defaults for creation-time conversion, and the live transaction now reflows
SpinBox arrow and ProgressRing preferred-size defaults as well. Explicit
SpinBox arrow sizes and ProgressRing width/height resources are tracked as
application-owned even when they match the old default. It does not
claim complete historical-widget reflow, per-monitor output selection,
automatic pixmap scaling, compositor scaling or physical-monitor-DPI
inference. Those require the separate XLibre-specific contract in WP-04A.

The deterministic live fixture also exercises the gallery's logical DPI
matrix: 96, 144, 192 and 288 DPI map to scales 1, 1.5, 2 and 3. It verifies rounded ToggleButton,
SpinBox and ProgressRing defaults at every step and reverses the transitions.

The installed fixture is `tests/automated/check-dpi.c`, run by
`tests/automated/run-widget-regressions.py`. It exercises 96, 192 and 384
DPI, malformed and non-finite values, saturation, and explicit-size
preservation under Xvfb.

In modern profiles, the scale transaction first stages toolkit-owned render
tables and appearance metrics for the affected subtree. If preparation fails,
no legacy scale callbacks run and the display override is rolled back. Logical
profile dimensions are converted once; explicit dimensions and captured classic
baselines remain device-pixel values. The role fixture compares live scaling
with fresh controls at 1x, 1.5x, 2x and 3x and checks preserved explicit fonts.

Applications can query `XmGetWidgetScale(widget, &scale)` from `<Xm/Display.h>`
for layout outside toolkit-owned dimensions, such as initial shell sizing.
It resolves the display override, then the widget's XLibre monitor scale, then
`Xft.dpi`, with a 1.0 fallback. Its minimum is 1.0, matching default dimension
scaling. Null arguments return False without writing an output value. The query
does not change resources or trigger a layout transaction.

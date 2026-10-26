# Stable as-needed scrollbar fit

An automatic ScrolledWindow with `XmAS_NEEDED` must keep a stable viewport when
its work area grows to fill the currently visible width or height.

When hiding a scrollbar, the layout recovers that scrollbar's highlight inset
on the other axis. Previously, the fit test used the smaller two-scrollbar
viewport but then exposed the larger viewport. A child sized to the exposed
area could cause the hidden bar to reappear, shrink the viewport, then disappear
again as the child followed the viewport. This also caused repeated configure
and expose events in responsive layouts.

Both fit tests now include the recovered inset before deciding to hide the
bar. Static-scrollbar layouts and the existing allocation formulas are unchanged.

Run `tests/automated/run-scrolled-window-fit.py --prefix INSTALL --out FRESH_DIR`.
The native test covers both axes, classic/light/dark/high-contrast profiles and
1/1.5/2/3 scales, repeatedly filling the visible axis and checking that both
viewport dimensions remain stable. Each case has an independent Xt display;
an anchor X connection prevents the test server resetting between cases.
The prior library fails the first classic case. Both Xft/SVG and
core-font/non-SVG development builds pass all 32 cases after the correction.

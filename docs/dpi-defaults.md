# Global DPI defaults

ck-core-motif reads the optional `Xft.dpi` X resource when a widget needs a
scaled default. The value is accepted only when it is finite, positive and
fully consumed after surrounding whitespace. The scale is `Xft.dpi / 75`,
matching the historical Motif baseline.

Only toolkit-owned defaults are scaled. Explicit application resources remain
authoritative: an explicit `XmNarrowSize` or `XmNindicatorSize` is returned
unchanged at every DPI. Invalid values fall back to the existing unscaled
default, and an oversized finite result saturates at the maximum `Dimension`
instead of wrapping during conversion.

The current qualified scope is deliberately global-per-display. It covers
unlabelled ToggleButton and ToggleButtonGadget indicators and SpinBox arrow
defaults. It does not claim live reflow, per-monitor output selection,
automatic pixmap scaling, compositor scaling or physical-monitor-DPI
inference. Those require the separate XLibre-specific contract in WP-04A.

The installed fixture is `tests/automated/check-dpi.c`, run by
`tests/automated/run-widget-regressions.py`. It exercises 75, 150 and 300
DPI, malformed and non-finite values, saturation, and explicit-size
preservation under Xvfb.

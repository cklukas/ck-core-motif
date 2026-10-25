---
nav_exclude: true
---
# ToggleSwitch redisplay guards

`XmToggleSwitch` now tolerates failed track, knob, or insensitive GC creation.
It uses an available GC for the knob and label, and skips rendering safely when
none exists. Focus rectangles are also omitted when the widget is too small to
represent their requested thickness, avoiding unsigned geometry underflow.

The focused source contract is checked by
`tests/automated/check-toggle-switch-redisplay-guards.py`.

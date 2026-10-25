# InfoBar hit-zone bounds

InfoBar reserves fixed-width close and action zones.  On a narrow widget, the
old `width - zone` comparisons became negative and treated almost every
button press as a command.  The event handler now clamps each zone origin to
zero, so the full narrow bar is a deliberate target only when its corresponding
control is present.

`tests/automated/check-infobar-hit-zones.py` keeps the clamped hit-zone
comparisons in the event path.

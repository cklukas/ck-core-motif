---
nav_exclude: true
---
# XmTimePicker

`XmTimePicker` edits a wall-clock value (`hour`, `minute`, `second`) in the
closed ranges 0–23, 0–59, and 0–59.  The value is not a timestamp: it has no
date, locale time zone, daylight-saving rule, or implicit conversion to an
instant.  Applications that attach a date and zone must resolve gaps and
overlaps explicitly before committing an instant.

The default range is 00:00:00 through 23:59:59.  `XmTimePickerSetRange`
accepts only valid, ordered bounds and rejects a range that excludes the
current value.  Invalid updates have no partial effect.

The default presentation is 24-hour `HH:MM:SS`; `XmNtimePickerFormat24Hour`
may request a deterministic 12-hour `AM`/`PM` presentation.  Left/Right moves
between hour, minute, and second fields.  Up/Down changes the focused field by
one unit, clamped at the documented range.  Return activates the value.
Callbacks are delivered after the value has been committed and may destroy
the widget.

# ProgressBar redisplay guards

`XmProgressBar` now tolerates partial GC allocation. It updates only existing
GCs, skips rendering when the track or fill GC is unavailable, and falls back
to the track GC for text. Focus rectangles are omitted when the widget is too
small for their requested thickness, avoiding unsigned geometry underflow.

The focused source contract is checked by
`tests/automated/check-progress-bar-redisplay-guards.py`.

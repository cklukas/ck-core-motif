# NumberBox redisplay guards

`XmNumberBox` now returns safely when one of its drawing GCs or its queried X11
font is unavailable. Those resources can fail independently during display
allocation; the previous path dereferenced the font before checking it and
could pass a null GC to Xlib drawing calls.

The focused source contract is checked by
`tests/automated/check-number-box-redisplay-guard.py`.

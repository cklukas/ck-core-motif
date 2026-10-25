# NumberBox redisplay guards

`XmNumberBox` now returns safely when one of its drawing GCs or its queried X11
font is unavailable. Those resources can fail independently during display
allocation; the previous path dereferenced the font before checking it and
could pass a null GC to Xlib drawing calls.

The redisplay path now reads the font resource from the active drawing GC with
`XGetGCValues(..., GCFont, ...)`. It never passes a GC context ID to
`XQueryFont`, avoiding `BadFont` on servers whose X11 resource namespaces do
not overlap.

The focused source contract is checked by
`tests/automated/check-number-box-redisplay-guard.py`.

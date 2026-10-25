# InfoBar redisplay guards

`XmInfoBar` now treats a missing drawing GC or an unavailable X11 font as a
non-rendering state and returns safely from `Redisplay`. X11 can report either
allocation failure, and neither result is safe to dereference. The normal
rendering path and callback behavior are unchanged.

The focused source contract is checked by
`tests/automated/check-info-bar-redisplay-guard.py`.

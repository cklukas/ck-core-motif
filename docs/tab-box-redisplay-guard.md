# XmTabBox live redisplay guard

`XmTabBox` keeps a drawing GC for its canvas. During redisplay it compares the
GC's background with the widget's current background and releases/recreates the
GC when an appearance profile changed the resource. The local drawing handle
must be replaced at the same time. Retaining the released handle makes the next
`XFillRectangle` use an invalid server-side GC and produces `BadGC`.

The fix updates the local handle immediately after `XtGetGC` returns the
replacement. The focused installed consumer
`tests/automated/run-tab-box-redisplay.py` realizes a real `XmTabBox`, forces a
classic expose, applies modern-light and modern-dark live profiles, drains the
X event queue after each transaction, and fails on any asynchronous X error.
The source guard is `tests/automated/check-tab-box-redisplay-guard.py`.

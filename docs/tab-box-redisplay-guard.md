# XmTabBox font ownership and live redisplay

`XmTabBox` owns a private graphics context for its canvas. Tab drawing changes
foreground colors, clip regions and fill styles, so the context must not be
shared through the Xt GC cache. Redisplay updates the existing context in place;
it does not release a context still referenced by the active drawing operation.
The context is freed once when the widget is destroyed.

Both `fontList` and `renderTable` updates retain their own render-table handle.
Previously, an accidental comparison discarded the result of `XmFontListCopy`,
leaving the widget with a borrowed handle and leaking the copy. Releasing the
caller's table or changing appearance could then invalidate the widget's table
and cause a teardown failure. A null assignment restores an owned copy of the
normal inherited default.

The installed consumer `tests/automated/run-tab-box-redisplay.py` realizes an
actual TabBox, forces classic and modern light/dark redisplay, and fails on
asynchronous X errors. It also alternates both font aliases, releases each
caller-owned table immediately, verifies usable text extents, restores defaults,
and destroys the widget. The ownership assertion fails against the previous
ABI 8 library and passes with the fix; the same fixture passes in the ABI 9
development build. The source guard checks that this mutable context no longer
uses the Xt shared-GC cache. These checks do not establish full tab visual-design
acceptance.

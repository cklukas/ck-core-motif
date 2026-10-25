---
nav_exclude: true
---
# Scaled-pixmap screen fallback

`_XmGetScaledPixmap` accepts either a `Screen` or a widget from which to
resolve one.  It previously called `DisplayOfScreen(screen)` before applying
that fallback, so a null screen could crash even when a widget was supplied;
a null screen and null widget also reached the dereference.

The entry point now resolves only when a widget is present, rejects an
unresolvable request, and obtains the display afterward.  The compatibility
entry points and normal screen path are unchanged.

`tests/automated/check-image-screen-fallback.py` protects this contract.

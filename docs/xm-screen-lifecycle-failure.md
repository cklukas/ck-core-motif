---
nav_exclude: true
---
# Xm screen lifecycle failure handling

Screen initialization now checks `XmScreenInfo` allocation before field
access. Screen destruction skips absent scratch and in-use hash tables, so a
partially initialized screen can still be torn down safely. Normal initialized
screens retain the existing cleanup callbacks.

The source-level regression is
`tests/automated/check-xm-screen-lifecycle-failure.py`.

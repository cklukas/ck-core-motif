---
nav_exclude: true
---

# Drag/drop argument-array bounds

`XmeDragSource` and `XmeDropSink` append toolkit-owned arguments to an
application-provided `ArgList`. Their count checks now bound the complete
byte size against both host `size_t` and Xt's `Cardinal` allocation width
before calling `XtMalloc`, preventing truncation on wide-size platforms.

The focused regressions are
`tests/automated/check-drag-source-allocation.py` and
`tests/automated/check-drop-sink-allocation.py`.

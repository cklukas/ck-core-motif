# Drop-site warning formatting

`_XmDSIGetChildPosition` reports inconsistent drop-site parentage using the
two widget resource names.  Resource names are caller-controlled strings, so
the diagnostic must not assume that the names fit in a fixed stack buffer.

The warning now uses `snprintf` with the existing 256-byte buffer.  Normal
Motif names are unchanged; unusually long names are safely truncated rather
than corrupting the stack or the drop-site manager's state.

The source regression in
`tests/automated/check-drop-site-warning-bounds.py` keeps both diagnostic
paths bounded and prevents an unbounded `sprintf` from being reintroduced.

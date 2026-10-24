# Mrm set-values reference list inputs

Set-values widget and callback reference builders now reject a null output-list
pointer before attempting initialization. The widget-reference path also
rejects a null widget name, preserving valid descriptor creation while
avoiding malformed-call crashes.

The source-level regression is
`tests/automated/check-mrm-wref-list-input.py`.

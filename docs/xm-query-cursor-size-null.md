# Xm cursor-size query validation

`XmeQueryBestCursorSize` now rejects null widgets or output pointers and
returns safely when no `XmScreen` is available. Valid queries retain the
existing maximum cursor dimensions.

The source-level regression is
`tests/automated/check-xm-query-cursor-size-null.py`.

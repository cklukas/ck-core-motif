---
nav_exclude: true
---
# Render-table AS_IS sentinel parsing

Render-table property import now compares the parsed integer for background,
foreground, underline, and strikethrough fields against the `-1` AS_IS
sentinel.  The previous code compared the token-type enum to `-1`, so the
sentinel was never recognized and could be passed through as a normal resource
value.  Other valid integer values retain their existing behavior.

The regression check is
`tests/automated/check-render-table-as-is-sentinel.py`.

---
nav_exclude: true
---
# Mrm name lookup validation

Mrm widget-class and global-name lookup now reject null names and null output
pointers before entering their hash tables. A supplied global-name output is
cleared before an invalid or absent name can return, so callers cannot retain a
stale value after a failed lookup. The class-record lookup also treats a null
name as absent, and its not-found diagnostic uses bounded formatting. Valid
class registration and lookup behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-lookup-input.py`.

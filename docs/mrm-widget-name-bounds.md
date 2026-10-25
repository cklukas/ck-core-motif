---
nav_exclude: true
---
# Mrm widget-name lookup bounds

Widget-instance lookup prepends `*` to an uncompressed class name for
`XtNameToWidget`. The temporary buffer now checks the terminator-inclusive
size arithmetic and copies the prefix and string directly, preserving the
lookup spelling while avoiding formatted-output overflow and size wrapping.

The source-level regression is
`tests/automated/check-mrm-widget-name-bounds.py`.

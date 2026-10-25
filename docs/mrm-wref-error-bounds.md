---
nav_exclude: true
---
# Mrm widget-reference diagnostic bounds

Set-values widget-reference construction reports unknown compressed argument
tags through a fixed 300-byte diagnostic buffer. Both widget and callback
reference paths now use `snprintf` with the destination size, preserving the
existing error handling while preventing an oversized tag value from
overrunning the stack buffer.

The source-level regression is
`tests/automated/check-mrm-wref-error-bounds.py`.

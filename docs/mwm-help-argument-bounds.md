---
nav_exclude: true
---

# MWM help-argument bounds

`WmDtGetHelpArgs` keeps its existing ABI and `MAXWMPATH + 1` output-buffer
contract.  It now validates all required pointers, initializes successful
outputs deterministically, and accepts a parsed topic or volume only when it
fits that established limit.  Oversized tokens are rejected with an empty
output instead of overflowing the caller's buffer.

Well-formed help arguments retain their original topic, volume, and argument
count. The focused source-level regression is
`tests/automated/check-mwm-help-argument-bounds.py`.

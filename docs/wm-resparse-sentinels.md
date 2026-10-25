---
nav_exclude: true
---
# WmResParse table sentinels

The Mwm resource parser uses sentinel entries in modifier and event tables.
Those entries previously cast the pointer-valued `NULL` macro into integer and
function-pointer fields, which triggers 64-bit compiler diagnostics and can
obscure real parser warnings.

The sentinels now use integer zero and a null function pointer explicitly.
Table layout, parsing behavior and public interfaces are unchanged.  The
regression check is `tests/automated/check-wm-resparse-sentinels.py`.

---
nav_exclude: true
---
# XmString self-concatenation

`XmStringConcatAndFree` now handles the same XmString passed as both operands
by taking a second reference before any in-place conversion. This preserves the
function’s ownership contract and prevents aliasing-induced mutation or
assertion failure; distinct operands behave as before.

The regression check is `tests/automated/check-xmstring-self-concat.py`.

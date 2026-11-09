---
nav_exclude: true
---

# MWM Panelist function-argument validation

The exported Panelist `ParseWmFunctionArg` helper now clears a supplied output
argument and rejects missing parser/output pointers, negative function indexes,
indexes beyond the fixed MWM function table, and table entries without a parser.
Valid function indexes and their parsing behavior are unchanged.

The focused regression is
`tests/automated/check-mwm-panelist-function-arguments.py`.

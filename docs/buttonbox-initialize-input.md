---
nav_exclude: true
---

# ButtonBox initialization input

The ButtonBox initialization method now validates its request/set widgets
before reading geometry or writing default dimensions. Malformed lifecycle
calls return safely; valid ButtonBox construction retains its existing
one-pixel minimum defaults.

The focused source contract is checked by
`tests/automated/check-buttonbox-initialize-input.py`.

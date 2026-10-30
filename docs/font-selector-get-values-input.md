---
nav_exclude: true
---

# FontSelector value-hook input

`GetValuesHook` now validates the widget class and the `ArgList`/count inputs
before reading private FontSelector state. Malformed hook calls return safely;
normal resource queries and current-font packaging are unchanged.

The focused source contract is checked by
`tests/automated/check-font-selector-get-values-input.py`.

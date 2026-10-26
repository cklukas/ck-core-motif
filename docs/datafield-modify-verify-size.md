---
nav_exclude: true
---
# DataField modify-verify temporary sizes

The wide-character modify-verify path now checks string length, character
width, multiplication, and Xt allocation width before creating its temporary
multibyte buffer. Invalid capacities fail the verification safely instead of
wrapping into a small allocation.

Regression coverage: `tests/automated/check-datafield-modify-verify-size.py`.

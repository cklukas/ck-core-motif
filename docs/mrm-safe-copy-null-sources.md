---
nav_exclude: true
---
# Mrm safe-copy null sources

`Urm__CW_SafeCopyValue` now rejects missing source pointers before copying
callback strings, vectors, floating-point values, or wide-character data.
Zero-valued scalar arguments keep their existing behavior because the checks
apply only to pointer-backed representations.

For a Char8 vector, the copy span is authoritative: a nonempty span requires a
source table even when its item count is zero. This prevents a malformed empty
table from reaching the raw byte copy.

The source-level regression is
`tests/automated/check-mrm-safe-copy-null-sources.py`.

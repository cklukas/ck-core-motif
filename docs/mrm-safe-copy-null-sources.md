# Mrm safe-copy null sources

`Urm__CW_SafeCopyValue` now rejects missing source pointers before copying
callback strings, vectors, floating-point values, or wide-character data.
Zero-valued scalar arguments keep their existing behavior because the checks
apply only to pointer-backed representations.

The source-level regression check is
`tests/automated/check-mrm-safe-copy-null-sources.py`.

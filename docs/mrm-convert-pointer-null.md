# Mrm pointer-backed conversion null values

`Urm__CW_ConvertValue` now rejects null CString, address-name, icon-image, and
bitmap-file values before invoking toolkit or hierarchy helpers. Valid
pointer-backed resources retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-convert-pointer-null.py`.

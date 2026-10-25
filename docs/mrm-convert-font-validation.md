# Mrm font conversion validation

`Urm__CW_ConvertValue` now rejects null font records, invalid font-list
markers, over-sized font lists, and missing font strings before calling
`strlen` or the X font loaders. Valid font and font-list conversions retain
the existing display-cache behavior.

The source-level regression check is
`tests/automated/check-mrm-convert-font-validation.py`.

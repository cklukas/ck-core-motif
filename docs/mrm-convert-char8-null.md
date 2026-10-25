# Mrm Char8 conversion null sources

`Urm__CW_ConvertValue` now rejects a null Char8 value before passing it to
`XmStringGenerate` or `XtParseTranslationTable`. Valid strings and existing
conversion behavior remain unchanged.

The source-level regression is
`tests/automated/check-mrm-convert-char8-null.py`.

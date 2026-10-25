---
nav_exclude: true
---
# Font selector XLFD fallback bounds

The font selector has a compatibility fallback for scaled XLFD fonts whose
reported metrics have zero height. Malformed or unusually long XLFD names used
to be copied into fixed `BUFSIZ` arrays and combined with `sprintf`, allowing a
font name to corrupt the selector process.

The fallback now validates the separator before use, allocates the derived
parts from their actual lengths, and builds the replacement name with bounded
`snprintf`. User-facing error messages are also bounded; the existing font
selection and fallback behavior is preserved.

`tests/automated/check-font-selector-bounds.py` guards these source-level
contracts.

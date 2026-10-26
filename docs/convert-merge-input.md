---
nav_exclude: true
---
# Conversion-merge callback input

`XmeConvertMerge` now rejects a null conversion callback structure before
entering the process lock or reading merge state.  Malformed callback paths
therefore return safely without dereferencing incomplete conversion state.

The focused regression is `tests/automated/check-convert-merge-input.py`.

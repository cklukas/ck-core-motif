---
nav_exclude: true
---

# Text converter inputs

Text and TextField selection converters now reject missing widgets, selection
atoms, output pointers, and format/length storage before deriving display or
encoding state. The TextField converter also delays `XmeGetEncodingAtom` until
after validation, preventing malformed owner requests from dereferencing a
null widget.

The regression check is
`tests/automated/check-text-converter-inputs.py`.

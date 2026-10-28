---
nav_exclude: true
---
# TextField empty preedit overstrike

The wide-character preedit path now creates an owned empty string when an XIM
draw callback has no text payload but overstrike recovery data still needs to
be appended. This prevents `wcslen` from dereferencing a null buffer while
preserving the existing recovery behavior.

The callback also rejects null XIM client or draw records before reading their
fields.

Regression coverage: `tests/automated/check-textfield-preedit-empty-overstrike.py`.

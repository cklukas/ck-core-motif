---
nav_exclude: true
---
# Mwm numeric KeySym parsing

Numeric KeySym details in Mwm event bindings are parsed through the same
checked `StrToNum` helper used for numeric window-manager function arguments.
`StrToNum` reports success separately from the converted value, so
`ParseKeySym` now passes its output pointer explicitly and assigns the parsed
value only after conversion succeeds. Invalid or overflowing numeric details
continue to be rejected as `NoSymbol`.

The parser also passes the first token byte to `isdigit` as an unsigned
character, as required by the C character-classification interface. The
focused regression check is
`tests/automated/check-wm-resparse-keysym-numbers.py`; an x86 Mwm rebuild
compiles the real parser.

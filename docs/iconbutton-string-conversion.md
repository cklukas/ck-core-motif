---
nav_exclude: true
---
# IconButton string conversion bounds

`XmIconButton` converts its legacy character label to an `XmString` through an
Xt `XrmValue`. It now rejects a missing label or a label whose NUL-terminated
byte length cannot fit Xt's `Cardinal` source-size field before conversion.
Ordinary label conversion and fallback-to-widget-name behavior are unchanged.

The focused regression check is
`tests/automated/check-iconbutton-string-conversion.py`.

---
nav_exclude: true
---
# Text/encoding value conversion failure

Text selection conversion obtains a temporary string for text and encoding
requests before calculating its length.  The result is now checked before
`strlen`.  If source conversion fails, the request reports zero length and is
refused without dereferencing null storage.

The regression check is
`tests/automated/check-textsel-text-value.py`.

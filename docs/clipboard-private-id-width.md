---
nav_exclude: true
---
# Clipboard private-ID width

The CutPaste API represents its private identifier as `long`, while target
lists carry X11 `Atom` values. The deferred clipboard path now performs that
widening explicitly when registering snapshot requests, preserving the
32-bit-compatible protocol value without an implicit conversion warning.

The focused regression is
`tests/automated/check-clipboard-private-id-width.py`.

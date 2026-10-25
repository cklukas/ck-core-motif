---
nav_exclude: true
---

# Clipboard format-unit validation

Clipboard format metadata stores the X11 property unit size. The metadata
reader now accepts only 8, 16, or 32 bits, returning the existing default
8-bit fallback for unsupported values. This prevents malformed metadata from
reaching property replacement and copy arithmetic while preserving valid
Motif formats.

The focused regression check is
`tests/automated/check-clipboard-format-unit-validation.py`.

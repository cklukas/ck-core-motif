---
nav_exclude: true
---
# Mrm header diagnostic bounds

UID header-version mismatch and parse diagnostics use a fixed 300-byte buffer.
Those three formatting sites now use `snprintf`, preserving normal messages
while safely truncating malformed or unusually long on-disk version text.

The source-level regression is
`tests/automated/check-mrm-header-error-bounds.py`.

---
nav_exclude: true
---
# Mrm resource-tag lookup failure handling

When compressed resource-tag lookup fails, `Urm__CW_CreateArglist` now skips
the malformed argument immediately after reporting the lookup error.  It no
longer falls through to `strcmp` with an unset resource-name pointer.  Valid
compressed and unknown tags retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-resource-tag-lookup-failure.py`.

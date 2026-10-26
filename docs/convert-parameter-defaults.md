---
nav_exclude: true
---

# Conversion callback parameter defaults

`_XmConvertHandler` can run callbacks without an active X selection request,
including local conversion paths.  The callback structure now initializes its
parameter payload to `NULL`, length zero, 8-bit format, and `None` type before
any request-specific overrides.  Convert callbacks therefore see deterministic
empty parameters instead of stack residue.

The regression check is
`tests/automated/check-convert-parameter-defaults.py`.

---
nav_exclude: true
---
# Standard conversion defaults

`XmeStandardConvert` is commonly called as the final converter in a callback
chain.  It now clears `value`, `type`, `format`, and `length` before examining
the target.  Unsupported targets and widgets without a render table therefore
produce a deterministic empty conversion instead of accidentally reusing
fields left by an earlier conversion.

The focused regression is
`tests/automated/check-standard-convert-defaults.py`.

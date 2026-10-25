---
nav_exclude: true
---
# Mrm module descriptor size bounds

Interface-module construction now keeps descriptor sizing in `size_t`, checks
the variable-count multiplication and full-word alignment, and rejects sizes
larger than the 16-bit Mrm resource limit before resizing the context. Valid
module counts retain their existing layout and initialization behavior.

The source-level regression is
`tests/automated/check-mrm-module-size-bounds.py`.

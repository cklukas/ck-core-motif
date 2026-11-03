---
nav_exclude: true
---
# Mrm module descriptor size bounds

Interface-module construction now keeps descriptor sizing in `size_t`, checks
the variable-count multiplication and full-word alignment, and rejects sizes
larger than the 16-bit Mrm resource limit before resizing the context. Valid
module counts retain their existing layout and initialization behavior.

The legacy `MrmCount` parameter is signed.  Negative values are rejected
before descriptor-size arithmetic, so an out-of-range caller-side conversion
cannot become a huge `size_t` allocation request.

The source-level regression is
`tests/automated/check-mrm-module-size-bounds.py`.

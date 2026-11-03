---
nav_exclude: true
---
# Mrm byte-swapped index-count bounds

Before byte-swapping each entry of a cross-endian IDB index leaf or node,
Mrm now validates the swapped signed count against the number of entries that
fit in the fixed-size database record. Malformed counts fail with the existing
`MrmFAILURE` result rather than causing the conversion loop to write past the
record buffer. Valid cross-endian UID index records retain their existing
conversion behavior.

The focused source regression is
`tests/automated/check-mrm-swap-index-count-bounds.py`.

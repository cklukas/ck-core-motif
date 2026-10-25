---
nav_exclude: true
---

# Subresource transformation bounds

`_XmTransformSubResources` now validates the resource-count multiplication
before copying compiled subresources. An unrepresentable result returns an
empty resource list, matching its existing allocation-failure behavior and
preventing size wraparound.
Null or zero-length input lists are also treated as empty results before any
resource entry is inspected.

The focused regression check is
`tests/automated/check-transform-subresources-bounds.py`.

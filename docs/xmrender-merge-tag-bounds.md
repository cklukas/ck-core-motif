---
nav_exclude: true
---
# XmRenderTable rendition-merge tag bounds

Rendition merging accepts an unsigned tag count. The reverse traversal now
counts down from that unsigned value instead of narrowing `tag_count - 1` to an
`int`, and a nonzero count requires a tag array. This preserves normal merge
ordering while avoiding implementation-defined large-count conversions and
null-array dereferences.

The source contract is checked by
`tests/automated/check-xmrender-merge-tag-bounds.py`.

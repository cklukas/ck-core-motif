---
nav_exclude: true
---
# Text-property concatenation bounds

When an X text property contains several items, Motif joins them into one
string. The byte total is accumulated with `size_t` and checked before adding
each item and the terminating byte. An impossible allocation returns
`XNoMemory` after releasing the X text list instead of wrapping an `int` and
allocating a buffer that is too small.

Compound-string decomposition also reserves the terminating NUL when joining
segments, and its fallback path keeps lengths in `size_t`. Unrepresentable
sizes return `XNoMemory` without writing beyond the result buffer.

The source-level regression is
`tests/automated/check-text-property-concat-bounds.py`.

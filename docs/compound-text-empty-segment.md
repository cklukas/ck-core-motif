---
nav_exclude: true
---

# Compound-text empty-segment guard

Compound-text decoding can produce an empty segment before a separator.  The
non-paired charset fallback previously inspected `ctx->item[0]` unconditionally
and could read beyond a zero-length segment.

The encoder now emits the empty segment through the existing concatenation path
and preserves separator behavior.  Non-empty segments and the public Motif ABI
are unchanged.  The static regression check is
`tests/automated/check-compound-text-empty-segment.py`.

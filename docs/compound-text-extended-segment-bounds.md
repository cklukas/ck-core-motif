---
nav_exclude: true
---

# Compound-text extended-segment bounds

The compatibility compound-text parser scans an extended segment for its
charset/text separator.  Malformed input without that separator previously
continued past the declared segment length.

The scan now stops at the segment boundary and rejects the segment when the
separator is absent.  The two-octet length header is also checked before
access.  Valid extended segments and the public Motif ABI are unchanged.  The static regression check is
`tests/automated/check-compound-text-extended-segment-bounds.py`.

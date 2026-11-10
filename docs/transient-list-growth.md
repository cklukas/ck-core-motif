---
nav_exclude: true
---

# MWM transient-window list growth

Transient-tree enumeration grows a temporary window-ID list as it walks
siblings and children. Growth now checks the size arithmetic and propagates
`XtRealloc` failure through the recursive walk; the top-level builder frees its
partial list and returns `NULL`. The remanagement path validates the client and
screen first, then stops before unmanaging anything when that list cannot be
built. The recursive helper also rejects incomplete list state rather than
writing through it. Enumeration now uses an owned iterative stack, preserves
the prior node/sibling/child order, and rejects a repeated client window. A
cyclic or excessively deep client-supplied transient graph therefore fails
without recursion or partial remanagement.

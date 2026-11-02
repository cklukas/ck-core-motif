---
nav_exclude: true
---

# Mrm FetchSetValues argument counts

`MrmFetchSetValues` accepts a public `Cardinal` count but uses an internal
pointer list and loop counters represented as `int`.  It now rejects counts
larger than `INT_MAX` before allocation or conversion to those internal
interfaces.  Representable argument lists retain their existing behavior;
unrepresentable counts return `MrmFAILURE` deterministically.

The focused regression is
`tests/automated/check-mrm-fetch-set-values-argument-count.py`.

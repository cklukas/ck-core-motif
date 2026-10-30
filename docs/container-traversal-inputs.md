---
nav_exclude: true
---
# Container traversal input validation

First-node lookup now handles an absent Container or empty node list, and
next-node traversal rejects nodes without widgets or parent Containers before
following layout-specific links. Valid tree traversal is unchanged.

The source contract is checked by
`tests/automated/check-container-traversal-inputs.py`.

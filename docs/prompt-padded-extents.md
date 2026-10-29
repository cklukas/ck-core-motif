---
nav_exclude: true
---
# Prompt padded extents

Prompt derives its preferred size from rendered text plus layout padding.
Initialization and geometry queries now use one saturating helper for both
width and height, preventing a very large rendered height from wrapping before
the padding is added.

The contract is protected by
`tests/automated/check-prompt-padded-extents.py`.

---
nav_exclude: true
---
# Input-method preedit width bounds

The input-method setup path now computes preedit width from a signed available
width after the status area and clamps it at zero.  A status area wider than a
collapsed shell therefore cannot wrap the preedit width into a large unsigned
value.

The source-level regression is
`tests/automated/check-xmim-preedit-width-bounds.py`.

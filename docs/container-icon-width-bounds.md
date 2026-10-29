---
nav_exclude: true
---
# Container icon-width edge arithmetic

`ResizeIconWidths` derives a common right edge from the current allocation,
margins, icon-header geometry, and queried child widths.  Those inputs mix
signed X11 positions with unsigned Motif dimensions.  Computing the edge or a
child width directly in `Position`/`Dimension` arithmetic could wrap on a tiny
allocation, producing a very wide icon or a negative-looking right-to-left
origin.

The routine now keeps edge calculations signed and widens each queried edge
before comparison.  Available child widths are saturated to `Dimension` only
after checking that the edge is positive, so an undersized allocation yields a
zero width rather than wrapped geometry.  The contract is protected by
`tests/automated/check-container-icon-width-bounds.py`.

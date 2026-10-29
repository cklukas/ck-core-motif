---
nav_exclude: true
---
# Container outline-detail position bounds

Outline-detail placement combines unsigned widget dimensions and margins with
signed X11 positions, depth indentation, and outline-button widths.  The old
right-to-left calculations performed those operations before converting to
`Position`, so a small or transient allocation could wrap an item to the wrong
side of the Container.

`LayoutOutlineDetail` now widens edge calculations and uses the existing
saturating position helpers for indentation, outline-button offsets, first
column limits, and final child placement.  Normal geometry is unchanged; values
outside the representable X11 position range clamp instead of wrapping.  The
contract is protected by `tests/automated/check-container-outline-position-bounds.py`.

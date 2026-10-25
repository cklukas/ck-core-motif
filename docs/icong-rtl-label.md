---
nav_exclude: true
---
# IconG right-to-left label placement

IconG now computes the right-to-left label origin in signed arithmetic before
converting it to `Position`.  Labels wider than a collapsed gadget remain
off-edge instead of wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-icong-rtl-label.py`.

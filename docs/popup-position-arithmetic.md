---
nav_exclude: true
---

# Anchored popup position arithmetic

`XmAutoSuggestBox` and `XmFlyout` now calculate anchor, offset, and screen-edge
coordinates in `long long` values.  They clamp the result before converting to
Xt's signed `Position` type, so large virtual screens and resource offsets no
longer wrap a popup around the display.

`tests/automated/check-popup-position-arithmetic.py` protects the source-level
contract.  The installed-prefix AutoSuggestBox and Flyout Xvfb fixtures remain
the runtime coverage for popup creation, placement, and dismissal.

---
nav_exclude: true
---
# Toolbar and StatusBar layout totals

Toolbar and StatusBar selection compare accumulated child widths with the
available widget width.  On 32-bit platforms, an unusually large item set could
wrap the `unsigned long` total and make an item appear to fit.  Both widgets
now saturate each width-plus-spacing addition at `ULONG_MAX` before comparing
or placing items.  Normal-sized layouts retain their previous geometry.

The contract is protected by
`tests/automated/check-composite-layout-total-bounds.py`.

---
nav_exclude: true
---
# Toolbar and StatusBar layout totals

Toolbar and StatusBar selection compare accumulated child widths with the
available widget width.  On 32-bit platforms, an unusually large item set could
wrap the `unsigned long` total and make an item appear to fit.  Both widgets
now saturate each width-plus-spacing addition at `ULONG_MAX` before comparing
or placing items.  Normal-sized layouts retain their previous geometry.
Scratch-array byte counts are also checked against Xt's `Cardinal` allocation
width before `XtCalloc` is called.
Preferred-size queries and internal-child mapping are also null-safe, so a
failed child creation cannot crash a later relayout.

The contract is protected by
`tests/automated/check-composite-layout-total-bounds.py`.

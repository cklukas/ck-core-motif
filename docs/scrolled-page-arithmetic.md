---
nav_exclude: true
---
# ScrolledWindow page arithmetic

Page actions derive their target from the current horizontal or vertical origin
and either a scrollbar page increment or the work-area dimension.  These values
are stored in `int`; adding or subtracting without widening can overflow before
`MoveWindow` applies the configured range.

Page-left/right/up/down now perform the arithmetic in `long long` and saturate
to `int` before range clamping.  Normal scrolling is unchanged, while extreme
content sizes cannot reverse a page action through integer overflow.  The
contract is protected by `tests/automated/check-scrolled-page-arithmetic.py`.

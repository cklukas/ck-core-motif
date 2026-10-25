---
nav_exclude: true
---
# XmeClearBorder bounds

`XmeClearBorder` now clamps the requested border thickness to the available
width and height before computing bottom and right clear origins.  Oversized
borders therefore clear only the widget-sized area instead of wrapping their
origins through unsigned subtraction.

The source-level regression is `tests/automated/check-clearborder-bounds.py`.

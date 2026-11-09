---
nav_exclude: true
---
# DropDown popup position bounds

DropDown popup placement now keeps translated coordinates, popup offsets, and
screen-fit arithmetic in `int`/wide expressions. The requested popup width and
final shell coordinates are explicitly clamped at Motif's legacy `Dimension`
and `Position` boundaries, preventing wrapped popups on large/high-DPI
desktops.

The focused regression is
`tests/automated/check-dropdown-popup-position-bounds.py`.

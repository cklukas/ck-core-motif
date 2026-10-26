# Text RTL autoscroll edge bounds

The RTL left-edge autoscroll threshold now computes its line-width extent in
`int64_t` before comparing the pointer and storing the edge position. Large
line counts and widths cannot wrap the edge calculation into the opposite
side of the widget.

The focused source contract is checked by
`tests/automated/check-text-rtl-autoscroll-edge-bounds.py`.

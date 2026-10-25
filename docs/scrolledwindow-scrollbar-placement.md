# ScrolledWindow scrollbar placement bounds

Bottom/right scrollbar placement now computes the available coordinate in
signed arithmetic and clamps it to zero when a collapsed ScrolledWindow is
smaller than the scrollbar.  This prevents unsigned subtraction from wrapping
the scrollbar into a distant position while retaining the existing placement
for normally sized windows.

The source-level regression is
`tests/automated/check-scrolledwindow-scrollbar-placement-bounds.py`.

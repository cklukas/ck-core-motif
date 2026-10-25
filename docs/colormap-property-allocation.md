# WM_COLORMAP_WINDOWS allocation bounds

Mwm adds one slot for the client window while processing the
`WM_COLORMAP_WINDOWS` property. The row now allocates `(nitems + 1)` complete
`Window` and `Colormap` elements, checks the size before converting to Xt’s
`Cardinal` allocator, and cleans up either partial allocation. The colormap
flags array uses the same checked element count.

`tests/automated/check-colormap-property-allocation.py` guards this contract.

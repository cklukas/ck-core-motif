# ClipWindow translation-buffer growth

Virtual-key translation assembly now handles an empty initial capacity,
checks `XtRealloc` before publishing the replacement, and aborts cleanly when
the initial buffer or a growth allocation fails. The previous zero-capacity
path could loop forever, while failed growth could lose the live buffer before
a write.

The regression check is
`tests/automated/check-clip-window-translation-allocation.py`.

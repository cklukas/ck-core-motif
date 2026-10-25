# ClipWindow translation-buffer growth

Virtual-key translation assembly now handles an empty initial capacity, checks
`XtRealloc` before publishing the replacement, and aborts cleanly when the
initial buffer or a growth allocation fails.  The previous zero-capacity path
could loop forever, while a failed growth could lose the live buffer before a
write.

`tests/automated/check-clip-window-translation-allocation.py` protects these
contracts.

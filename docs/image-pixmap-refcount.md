# ImageCache pixmap reference counts

`XmDestroyPixmap` releases a cached pixmap by decrementing its reference
count.  A stale or duplicate release now rejects an already-zero or invalid
count before decrementing, preserving cache ownership and avoiding integer
underflow.

`tests/automated/check-image-pixmap-refcount.py` protects this lifecycle
invariant.

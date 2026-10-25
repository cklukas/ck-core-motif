# Container cell-array growth

When a Container changes only its minor layout dimension, the cell array is
grown in place.  Growth now checks the cell-count arithmetic and uses a
temporary pointer for `XtRealloc`; allocation failure leaves the existing cell
array and count intact, so the next layout pass can retry safely.

The focused regression is `tests/automated/check-container-cell-growth.py`.
It checks the overflow guards, temporary allocation, failure check, and the
absence of the old direct assignment pattern.

# XmScreen child-array growth

`XmScreen` keeps its desktop children in a growable array. Insertion now
checks slot arithmetic and both host `size_t` and Xt `Cardinal` byte limits,
and grows through a temporary pointer, so an Xt allocation failure leaves the
existing children and slot count valid and skips only the new insertion.

The focused regression is `tests/automated/check-screen-child-growth.py`.

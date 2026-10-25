# XmScreen child-array growth

`XmScreen` keeps its desktop children in a growable array. Insertion now
checks the slot arithmetic and grows through a temporary pointer, so an Xt
allocation failure leaves the existing children and slot count valid and
skips only the new insertion.

The focused regression is `tests/automated/check-screen-child-growth.py`.

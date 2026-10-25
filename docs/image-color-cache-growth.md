# ImageCache color-cache growth

The color cache grows in fixed-size batches.  Growth now validates the count
and byte-size arithmetic, keeps the old cache until the replacement allocation
succeeds, and reports failure to the color lookup path.  A newly allocated X11
color is released when it cannot be cached, avoiding both a null write and a
resource leak under allocation pressure.

`tests/automated/check-image-color-cache-growth.py` protects this contract.

# Scaled-image strip endpoint safety

`_XmPutScaledImage` renders large images in vertical strips.  The final strip
may start close to the largest representable unsigned coordinate, so adding
the nominal strip height before checking the remaining range can wrap and
produce an invalid endpoint.  The renderer now clamps the strip height from
the remaining distance first, then performs the addition.

The source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-image-scaling-strip-bound.py
```

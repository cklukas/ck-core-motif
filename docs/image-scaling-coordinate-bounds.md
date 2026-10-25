# Image scaling destination coordinates

The private scaled-image path accepts signed X11 destination coordinates so an
image can be clipped at the edge of a drawable.  Its strip loop now performs
the `dest_y + min_y` calculation in a wide signed type and checks the result
before converting back to the X11 `int` coordinate.  A negative destination or
a destination close to `INT_MAX` therefore cannot wrap through unsigned
promotion and send a corrupted coordinate to `XPutImage`.

The existing dimension check still rejects a destination whose complete
vertical extent cannot be represented.  The source regression in
`tests/automated/check-image-scaling-coordinate-bounds.py` protects both the
wide calculation and the checked conversion.

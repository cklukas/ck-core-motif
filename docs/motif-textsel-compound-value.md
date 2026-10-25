# Text compound-text value allocation failure

Text selection conversion copies Xlib compound-text property bytes into an
Xt-owned buffer.  The destination allocation is now checked before copying.
If it fails, the Xlib property is released, the conversion reports zero
length, and the request is refused safely.

The regression check is
`tests/automated/check-textsel-compound-value-allocation.py`.

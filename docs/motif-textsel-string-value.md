# Text XA_STRING value allocation failure

Text selection conversion copies the Xlib text-property bytes into an Xt-owned
buffer for `XA_STRING` requests.  The destination allocation is now checked
before copying.  If it fails, the Xlib property is released, the conversion
reports zero length, and the request is refused safely.

The regression check is
`tests/automated/check-textsel-string-value-allocation.py`.

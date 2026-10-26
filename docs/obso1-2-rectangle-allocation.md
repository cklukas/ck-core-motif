# Legacy rounded-rectangle allocation

The compatibility drawing helpers in `Obso1_2.c` cache rounded-rectangle
segments in a growable `XRectangle` array.  Their size checks now cover both
`size_t` and Xt's `Cardinal` allocation range before the `size * 4`
multiplication.  Failed allocations continue to leave the existing cache
untouched.

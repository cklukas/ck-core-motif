# Empty Region shrink allocation

Region operations may shrink their rectangle storage after an intersection or
subtraction.  The non-empty shrink path now checks the rectangle-byte count
before narrowing it to Xt's `Cardinal` allocation size.  When the result is
empty, the replacement one-record allocation is obtained before the old array
is released.  If allocation fails or the size is unrepresentable, the region
keeps its existing (possibly larger) array and remains valid for later
operations instead of exposing a null rectangle pointer.

The focused source-level regression is
`tests/automated/check-region-empty-shrink-allocation.py`.

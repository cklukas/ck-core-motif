# Image-cache scaling table bounds

Image scaling allocates four coordinate and span tables through Xt.  The
allocation guards now require each table size to fit both native `size_t`
arithmetic and Xt's `Cardinal` allocation argument.  This avoids truncating a
large 64-bit dimension when it is passed to `XtMalloc`, while leaving normal
scaling unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-image-cache-table-bounds.py
```

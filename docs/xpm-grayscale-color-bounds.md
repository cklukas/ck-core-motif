# XPM grayscale color replacement bounds

`_XmConvertToBW` can receive XPM colors whose original strings are shorter
than the 13-character expanded grayscale form. It now allocates a replacement
string, formats it with a checked `snprintf`, and swaps it only after the new
value is ready. It first requires the original value to be exactly the
expanded 12-digit hex form, so short or malformed `#` values are not read past
their terminator. Allocation failure leaves the original color unchanged. This
avoids both over-reading and writing past short color-string allocations while
preserving the existing grayscale conversion.

The focused source contract is checked with:

```sh
python3 tests/automated/check-color-bw-replacement-bounds.py
```

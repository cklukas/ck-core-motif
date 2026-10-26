# XPM image pixel-index validation

`XpmCreateDataFromXpmImage` now validates every pixel index before looking up
the corresponding color string while serializing an `XpmImage`.  It also
checks the `width * cpp + 1` row-size arithmetic and reserves room for each
row terminator before writing.

Malformed images with an out-of-range pixel index, a missing color string, or
an overflowing row size return `XpmFileInvalid` instead of indexing beyond the
color table or writing past the generated data buffer.  Valid images retain
the existing serialized format and API behavior.

The focused source-contract check is:

```sh
python3 tests/automated/check-xpm-data-pixel-index-bounds.py
```

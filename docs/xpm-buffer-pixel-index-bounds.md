# XPM buffer pixel-index bounds

XPM buffer serialization now passes the image color count into its pixel
writer. Each pixel index is validated before indexing the color table, and
remaining-buffer arithmetic is checked before every row write. Malformed image
data therefore returns `XpmFileInvalid` instead of reading past the color
table or wrapping the output span.

The focused source contract is checked with:

```sh
python3 tests/automated/check-xpm-buffer-pixel-index-bounds.py
```

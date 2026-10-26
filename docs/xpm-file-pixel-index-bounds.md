# XPM file pixel-index validation

`XpmWriteFileFromXpmImage` now validates each image pixel index against the
image color table before copying its serialized color key.  A missing color
string is rejected as well.  Invalid images return `XpmFileInvalid` without
writing an out-of-bounds color-table entry; valid XPM output is unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-xpm-file-pixel-index-bounds.py
```

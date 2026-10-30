# XPM extension-size bounds

XPM buffer serialization now checks extension names and lines before summing
their lengths. Null extension storage and `unsigned int` addition overflow
return a failure before the output buffer is allocated; the caller propagates
that failure instead of using a wrapped size. The writer also tracks remaining
capacity for each extension line and the final `XPMENDEXT` terminator, so a
cursor inconsistency cannot turn an exact allocation into an out-of-bounds
copy.

The focused source contract is checked with:

```sh
python3 tests/automated/check-xpm-extension-size-bounds.py
python3 tests/automated/check-xpm-data-extension-output-bounds.py
```

# XPM comment-size bounds

XPM buffer serialization now accumulates hint, color, and pixel comments in a
`size_t` temporary with checked additions. Null metadata and values that cannot
fit the public `int` size contract fail before output allocation; the caller
propagates that failure instead of using a wrapped comment size.

The focused source contract is checked with:

```sh
python3 tests/automated/check-xpm-comment-size-bounds.py
```

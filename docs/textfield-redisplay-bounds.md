# TextField redisplay geometry bounds

`RedisplayText` now performs its margin and content-size checks in `uint64_t`
instead of allowing `Dimension` multiplication or addition to wrap. It also
rejects an unrepresentable baseline before passing it to the drawing path, so
oversized resources skip painting safely rather than using invalid geometry.

The focused source contract is checked by
`tests/automated/check-textfield-redisplay-bounds.py`.

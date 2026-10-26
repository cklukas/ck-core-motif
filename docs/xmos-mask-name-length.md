# Xmos mask-name length handling

`_XmOSGenerateMaskName` now keeps the image-name prefix length in `size_t`
instead of narrowing a pointer difference to `int`. It also returns safely for
missing input or output buffers before path parsing.

The focused source regression is
`tests/automated/check-xmos-mask-name-length.py`.

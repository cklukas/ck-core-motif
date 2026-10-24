# Xmos path construction bounds

The path helpers use caller-provided `MAX_DIR_PATH_LEN` storage. The previous
implementation copied a trusted-looking `PWD` environment value and appended
relative path text without checking either length.

`GetCurrentDir` now ignores an overlong `PWD` and falls back to `getcwd`, while
`_XmOSAbsolutePathName` checks both `..` replacement and ordinary relative-path
appends before copying. Valid paths retain their existing resolution behavior;
unrepresentable paths return failure without writing past the buffer.

`tests/automated/check-xmos-path-bounds.py` guards the capacity checks.

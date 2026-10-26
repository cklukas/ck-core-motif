# Xmos file-name construction bounds

`_XmOSBuildFileName` now rejects missing inputs and checks absolute and joined
path lengths before allocating the returned string. The checks cover both
`size_t` addition overflow and the maximum `Cardinal` allocation width.

The focused source regression is
`tests/automated/check-xmos-build-filename-bounds.py`.

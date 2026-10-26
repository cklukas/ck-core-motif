# XPM buffer color output bounds

The buffer-export serializer uses a fixed temporary line for each XPM color.
`WriteColors` now checks the remaining capacity and the `snprintf` result before
advancing the line pointer. A truncated symbolic-color description returns
`XpmNoMemory` without reading or forming a pointer beyond that buffer.

The focused source regression is
`tests/automated/check-xpm-buffer-colors-bounds.py`.

# XPM color output bounds

The XPM image serializer builds each color description in a fixed temporary
buffer. `CreateColors` now checks the remaining capacity and the return value
from `snprintf` before advancing its output pointer. A truncated description
returns `XpmNoMemory` without forming an out-of-bounds pointer.

The focused source regression is
`tests/automated/check-xpm-create-colors-bounds.py`.

# ScrollBar redisplay shadow bounds

ScrollBar redisplay now clamps the shadow drawing width and height to one pixel
when the widget is smaller than twice its highlight thickness.  Collapsed
controls no longer pass wrapped unsigned dimensions to the shadow renderer.

The source-level regression is
`tests/automated/check-scrollbar-redisplay-bounds.py`.

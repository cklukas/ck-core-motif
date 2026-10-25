# ToggleButton gadget highlight bounds

ToggleButton gadget menu-leave highlighting now passes signed inset dimensions
to `XmeDrawHighlight`.  A collapsed gadget therefore reaches the shared
zero-area guard instead of passing a wrapped `Dimension` span.

The source-level regression is `tests/automated/check-togglebg-highlight-bounds.py`.

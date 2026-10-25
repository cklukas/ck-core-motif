# PushButton gadget highlight bounds

PushButton gadget menu-disarm highlighting now passes signed inset dimensions
to `XmeDrawHighlight`.  Collapsed gadgets therefore reach the shared guard
instead of passing wrapped `Dimension` spans.

The source-level regression is `tests/automated/check-pushbg-menu-highlight-bounds.py`.

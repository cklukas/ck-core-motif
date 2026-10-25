# ScrollBar resize spans

ScrollBar arrow and slider spans now use signed, bounded arithmetic.  A
control that is smaller than its highlight and shadow insets keeps a one-pixel
span instead of wrapping an unsigned `Dimension` subtraction into a huge
rectangle.  The behavior is applied consistently to horizontal and vertical
orientation, with and without arrows.

The source-level regression is `tests/automated/check-scrollbar-resize-spans.py`.

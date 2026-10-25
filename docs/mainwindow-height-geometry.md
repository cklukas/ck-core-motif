# MainWindow child height geometry

MainWindow now derives parent heights from signed intermediates when a child
changes height.  Collapsed or shrinking children can no longer make the
unsigned `Dimension` subtraction wrap to a huge request.  The corresponding
scrolled-window Y offset update also uses a signed height delta, preserving the
content position during a shrink.

The source-level regression is
`tests/automated/check-mainwindow-height-arithmetic.py`.

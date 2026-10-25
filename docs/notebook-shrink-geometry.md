# Notebook shrinking geometry

Notebook layout reduces its remaining width or height as page, scroller,
binding, and tab regions are minimized.  Those values are toolkit
`Dimension`s; subtracting a region larger than the remaining space used to
wrap to a huge value and could make later child sizes grow during a shrink.

The shrinking path now saturates each subtraction at zero. Normal layouts keep
their existing sizes, while very small notebooks reduce regions in a
deterministic order without unsigned underflow.

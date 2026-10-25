# TabBox stack-count bounds

`XmNnumStacks` is an integer resource and applications can set it to zero or
a negative value.  Stacked layout divides the tab count by this value, so an
invalid resource could terminate the process with a divide-by-zero signal.
`CalcStackedGeometry` now treats values below one as one before doing layout
arithmetic, preserving the normal layout contract for valid values.

`tests/automated/check-tabbox-num-stacks.py` keeps the guard in the layout
path.

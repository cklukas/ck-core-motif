# Container cell-layout allocation

The cell-layout trial pass allocates a temporary occupancy grid from its
requested width and height.  It now checks positive dimensions and both
multiplications before allocating, returns safely when the grid cannot be
created, and stops before incrementing dimensions past `INT_MAX`.  Normal
layout placement and recursive fitting remain unchanged.

`tests/automated/check-container-cell-layout-allocation.py` protects these
bounds and the allocation-failure return.

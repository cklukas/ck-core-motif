---
nav_exclude: true
---
# Container CELLS region lifecycle

`XmCELLS` placement tracks occupied rectangles in a persistent X11 region.
That region can fail to allocate during widget creation, and teardown or a
later layout reset must tolerate the missing state.  CELLS hit testing and
placement now fail safely when the region is unavailable, while destruction
and reset skip null-region operations.  GRID and valid CELLS behavior remain
unchanged.

`tests/automated/check-container-cells-region-lifecycle.py` protects these
allocation and lifecycle contracts.

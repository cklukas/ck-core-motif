# Container cell-position guards

Cell coordinate conversion can run while a Container is between layouts.  The
helpers now reject zero cell metrics or zero current grid dimensions before any
division.  Coordinate-to-cell conversion returns `NO_CELL`; cell-to-coordinate
conversion returns the container margin origin.  A null output point is also
handled safely.

`tests/automated/check-container-cell-position.py` protects these fallback
contracts.

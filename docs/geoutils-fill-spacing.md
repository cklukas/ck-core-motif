# GeoUtils fill spacing guards

The private fill-spacing helper now handles empty rows and missing output
pointers without dividing by zero or writing through NULL.  Its spacing sum
uses `unsigned long` intermediates, avoiding `Dimension` wraparound when a row
contains many boxes or large spacing values.  Normal nonempty layouts retain
the existing proportional spacing behavior.

The regression is `tests/automated/check-geoutils-fill-guards.py`.

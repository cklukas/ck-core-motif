# Region shadow segment counts

Region shadow drawing allocates segment arrays proportional to the rectangle
count and requested shadow thickness.  The old calculation performed that
product in the region's `long` value and then narrowed it to `int`; extreme
geometry could wrap the count and under-allocate the arrays used while
drawing.

The code now validates the product against `INT_MAX` before allocating and
uses a widened multiplication for the valid case.  Ordinary shadows are
unchanged, while unrepresentable geometry emits the existing warning and
returns safely.  The source regression in
`tests/automated/check-region-shadow-counts.py` protects the check.

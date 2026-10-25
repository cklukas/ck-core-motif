# BreadcrumbBar size bounds

Breadcrumb labels and separators contribute fixed pixel estimates to hit
testing and preferred geometry.  The previous signed `int` arithmetic could
wrap for very long labels or many segments, producing negative cursor
positions.  Width calculation and cursor advancement now saturate at
`INT_MAX`, and preferred-width accumulation stops at that bound.

`tests/automated/check-breadcrumb-size-bounds.py` keeps the saturating
arithmetic in the sizing and hit-test paths.

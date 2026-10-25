# GeoUtils empty-row dimensions

`_XmGeoGetDimensions` now treats a row with zero child boxes as having no
between-box spacing.  The old `numBoxes - 1` expression underflowed for an
empty row and could produce a huge wrapped fill width.  The helper also
returns safely for an incomplete matrix descriptor.

Nonempty rows retain their existing spacing calculation.  The regression is
`tests/automated/check-geoutils-dimensions.py`.

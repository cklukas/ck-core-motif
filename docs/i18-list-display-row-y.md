# I18List display row Y bounds

`DisplayList` now computes its initial row baseline in wide arithmetic and
clamps it.  Each subsequent row advance is clamped as well, preventing large
row offsets or metrics from wrapping the `Position` used by fills and pixmap
copies.

The source-contract regression is
`tests/automated/check-i18-list-display-row-y.py`.

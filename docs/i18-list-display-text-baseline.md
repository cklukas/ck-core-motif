# I18List display text baseline

The text/icon rendering branch of `DisplayList` now computes its starting row
baseline in wide arithmetic and clamps both visual-style adjustments.  Large
row offsets can no longer wrap the baseline before text and pixmap rendering.

The source-contract regression is
`tests/automated/check-i18-list-display-text-baseline.py`.

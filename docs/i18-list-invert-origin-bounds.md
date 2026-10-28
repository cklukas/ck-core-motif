# I18List selection-inversion origin bounds

`InvertArea` now computes the full-row inversion origin in wide arithmetic and
clamps both right-to-left and left-to-right origins before drawing.  Wide
content or widget widths can no longer wrap the selection highlight origin
through the legacy short coordinate.

The source-contract regression is
`tests/automated/check-i18-list-invert-origin-bounds.py`.

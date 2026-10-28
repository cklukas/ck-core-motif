# I18List left-arrow scrolling bounds

The horizontal left-arrow callback now combines its partial-column
adjustments in wide arithmetic and clamps the resulting horizontal origin.
This avoids compound `short` updates wrapping when a list is positioned near
the representable coordinate limits.

The source-contract regression is
`tests/automated/check-i18-list-left-arrow-bounds.py`.

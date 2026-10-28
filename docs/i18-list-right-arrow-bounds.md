# I18List right-arrow scrolling bounds

The horizontal right-arrow callback now computes content/window differences
in wide arithmetic and clamps every resulting horizontal-origin update before
storing the legacy `short` value.  This prevents large content widths or
column edges from wrapping the scroll position.

The source-contract regression is
`tests/automated/check-i18-list-right-arrow-bounds.py`.

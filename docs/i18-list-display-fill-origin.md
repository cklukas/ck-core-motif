# I18List display fill origins

The selected-row and entry-background fills in `DisplayList` now use bounded
coordinate arithmetic for both layout directions.  Wide content and widget
widths can no longer wrap the fill origin before it reaches Xlib.

The source-contract regression is
`tests/automated/check-i18-list-display-fill-origin.py`.

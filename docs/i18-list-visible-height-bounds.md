# I18List visible-height bounds

`IsRowVisible` probes the bottom of the widget by passing its height plus one
to the row/column conversion helper.  That value is now clamped through the
shared bounded-short conversion instead of being narrowed directly.  Tall
widgets therefore cannot wrap the probe to a negative coordinate and report
the wrong visible row.

The source-contract regression is
`tests/automated/check-i18-list-visible-height-bounds.py`.

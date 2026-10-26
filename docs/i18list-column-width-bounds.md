---
nav_exclude: true
---
# I18List column-width allocation bounds

`XmI18List` derives its column count from widget state and uses that count to
allocate the `short` column-width array.  The allocation now rejects negative
counts and checks both `size_t` multiplication and Xt's `Cardinal` limit before
narrowing the byte count.  An unrepresentable request returns without
discarding the existing widths or writing through an invalid replacement.

The focused source regression is
`tests/automated/check-i18list-column-width-rollback.py`.

# I18List visibility movement bounds

`AdjustVisiblePosition` now negates movement values in wide arithmetic and
clamps them before updating the legacy `short` horizontal location.  This
avoids the signed-minimum negation and compound-assignment wrap that could
send a visibility adjustment in the wrong direction.

The source-contract regression is
`tests/automated/check-i18-list-adjust-position-bounds.py`.

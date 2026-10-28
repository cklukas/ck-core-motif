# I18List header and icon baselines

`DisplayList` now bounds the title and first-column-icon baseline setup,
including visual-style spacing adjustments.  Separator metrics that exceed the
legacy `Position` range can no longer wrap before header text or row pixmaps
are drawn.

The source-contract regression is
`tests/automated/check-i18-list-header-baselines.py`.

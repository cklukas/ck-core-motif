# I18List first-column restoration bounds

`AdjustFirstRowAndCol` now accumulates the restored horizontal origin in
`long long` and clamps both width-derived assignments before storing the
legacy `short` location.  This prevents wide column sets or widget widths
from wrapping the restored first-column position.

The source-contract regression is
`tests/automated/check-i18-list-first-column-bounds.py`.

# I18List display column origin

`DisplayList` now bounds its initial RTL and LTR column origins before walking
the columns.  A wide widget or horizontal offset can no longer wrap `cur_x`
before the per-column movement safeguards run.

The source-contract regression is
`tests/automated/check-i18-list-display-column-origin.py`.

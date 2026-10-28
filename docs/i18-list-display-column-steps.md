# I18List display column steps

`DisplayList` now clamps each RTL/LTR column advancement, including the
inter-column spacing step.  Wide column metrics can no longer wrap `cur_x`
before header, text, or pixmap drawing.

The source-contract regression is
`tests/automated/check-i18-list-display-column-steps.py`.

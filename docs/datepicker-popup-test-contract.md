---
nav_exclude: true
---
# DatePicker popup test contract

The DatePicker popup implementation uses wide `x_pos` and `y_pos` values
before clamping to the screen and converting to Motif `Position`. The focused
regression now checks those current contracts directly; it no longer expects
the removed narrow `Position` arithmetic.

The contract check is
`tests/automated/check-datepicker-popup-position-contract.py`.

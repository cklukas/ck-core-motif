# PushButton default-shadow erase bounds

`EraseDefaultButtonShadow` now skips border clearing for buttons no larger than
twice the highlight delta and performs the remaining subtraction with signed
operands.  Collapsed buttons cannot wrap into oversized clear rectangles.

The focused regression is `tests/automated/check-pushb-erase-shadow-bounds.py`.

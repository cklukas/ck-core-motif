# I18List text centering

Text centering in `DisplayList` now derives a signed wide offset before
updating the row baseline.  A text run taller than its row no longer causes
unsigned subtraction to underflow, and each baseline update is clamped before
the next draw.

The source-contract regression is
`tests/automated/check-i18-list-text-centering.py`.

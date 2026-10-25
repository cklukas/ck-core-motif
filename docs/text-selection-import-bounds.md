---
nav_exclude: true
---
# Text selection import bounds

Incoming selection lengths are Xlib `unsigned long` values, while Motif’s text
source block length is an `int`. Primary, clipboard, and drop imports now reject
lengths that cannot be represented before narrowing, free the transfer payload,
and report failure through the existing transfer lifecycle.
For primary imports, the existing `num_chars` hand-off is cleared so a
rejected payload cannot trigger the move/delete follow-up.

The source contract is covered by
`tests/automated/check-text-selection-import.py`.

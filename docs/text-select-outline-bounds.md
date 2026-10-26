# Text line-selection bounds

`SelectOutLine` now handles an empty line table without subtracting one from
zero, and saturates movement at the first or last representable line instead
of relying on signed interpretation of an unsigned line index. Negative or
zero movement counts remain no-ops, and the historical left-selection
one-line adjustment is preserved.

The focused source contract is checked by
`tests/automated/check-text-select-outline-bounds.py`.

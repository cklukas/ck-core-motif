# Text cursor-scroll parameter bounds

`ScrollCursorVertically` now treats its percentage action parameter as a
bounded 0–100 value. Invalid text, negative values, and values above 100 use
the nearest endpoint; an empty line table resolves to line zero. The line
calculation avoids multiplying a large line count by the percentage directly,
and event coordinates are saturated before the Position-based hit-test.

The focused source contract is checked by
`tests/automated/check-text-scroll-cursor-parameter-bounds.py`.

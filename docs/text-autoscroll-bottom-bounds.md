# Text autoscroll bottom bounds

`CheckTimerScrolling` now computes the text-area bottom in `int64_t` before
comparing motion coordinates. Both the interior test and LTR below-edge test
therefore remain correct when line height and line count produce a large
layout extent instead of wrapping in legacy integer arithmetic.

The focused source contract is checked by
`tests/automated/check-text-autoscroll-bottom-bounds.py`.

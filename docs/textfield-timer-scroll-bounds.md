# TextField timer-scroll edge bounds

TextField selection autoscroll now computes margin and text-edge thresholds in
`int64_t`. The left and right edge targets are clamped to the signed X11
`Position` range before being stored, preventing large margins or average
character widths from wrapping the timer-scroll position.

The focused source contract is checked by
`tests/automated/check-textfield-timer-scroll-bounds.py`.

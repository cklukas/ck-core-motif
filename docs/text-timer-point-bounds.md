# Text timer-scrolling point bounds

Text selection autoscroll now saturates motion-event coordinates before storing
them in the 16-bit input positions used by the timer path. This prevents large
or translated X coordinates from wrapping and sending the autoscroll cursor to
the opposite side of the widget.

The focused source contract is checked by
`tests/automated/check-text-timer-point-bounds.py`.

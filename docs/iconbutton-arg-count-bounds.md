# IconButton argument-count bounds

`XmIconButton::SetValues` now scans its Xt argument list with a `Cardinal`
counter matching `num_args`. Resource-change detection and button behavior are
unchanged while large valid argument lists avoid signed-counter wraparound.

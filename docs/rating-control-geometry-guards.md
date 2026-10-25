# RatingControl geometry guards

`XmRatingControl` now handles zero-width and very small widgets without
integer division by zero or unsigned rectangle-size underflow. It skips
segments that cannot fit and clamps the inner segment and focus rectangle to
the available geometry while preserving the normal layout at usable sizes.

The focused source contract is checked by
`tests/automated/check-rating-control-geometry-guards.py`.

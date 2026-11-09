---
nav_exclude: true
---
# RatingControl geometry guards

`XmRatingControl` now handles zero-width and very small widgets without
integer division by zero or unsigned rectangle-size underflow. It skips
segments that cannot fit and clamps the inner segment and focus rectangle to
the available geometry while preserving the normal layout at usable sizes.

The focused source contract is checked by
`tests/automated/check-rating-control-geometry-guards.py`.
# RatingControl drawing lifecycle

RatingControl ignores late highlight and unhighlight requests after Xt begins
destroying the widget.  This avoids changing primitive focus state or queuing a
buffered redraw against teardown state.

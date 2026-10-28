# Cropped image scaling Y origin

The scaled-image path now normalizes Y coordinates relative to `src_y`, in
parallel with its X handling. A nonzero vertical crop previously used the
full-image scaled Y coordinate when deciding whether to draw, which could
skip the crop entirely. The normalized bottom edge is bounded before drawing;
zero-origin calls retain their existing behavior.

Regression coverage: `tests/automated/check-image-scaling-source-origin-y.py`.

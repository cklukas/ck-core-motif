# Scale trailing tick bounds

Scale’s trailing tick coordinates now calculate widget and slider extents in
signed arithmetic.  Collapsed or temporarily over-constrained scales no
longer wrap a trailing tick position through unsigned `Dimension` subtraction.

The source-level regression is
`tests/automated/check-scale-trailing-ticks.py`.

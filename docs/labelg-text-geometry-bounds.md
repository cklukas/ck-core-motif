# LabelG text geometry bounds

LabelG now computes the right text edge in signed arithmetic before applying
alignment.  Narrow labels whose insets exceed their width therefore keep a
bounded negative/off-edge coordinate instead of wrapping to a distant positive
position.

The source-level regression is
`tests/automated/check-labelg-text-geometry-bounds.py`.

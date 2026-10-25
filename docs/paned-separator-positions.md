# Paned separator positions

Paned separator x and y coordinates now use signed intermediates before
conversion to `Position`.  Overfull or collapsed panes therefore keep valid
off-edge separator positions instead of wrapping mixed signed and unsigned
dimension arithmetic.

The source-level regression is
`tests/automated/check-paned-separator-positions.py`.

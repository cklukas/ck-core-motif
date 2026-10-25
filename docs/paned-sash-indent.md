# Paned negative sash indents

Paned sash placement now computes negative-indent coordinates in signed
arithmetic before converting to `Position`.  Narrow containers therefore keep
the intended off-edge sash location instead of wrapping to a distant positive
coordinate.

The source-level regression is `tests/automated/check-paned-sash-indent.py`.

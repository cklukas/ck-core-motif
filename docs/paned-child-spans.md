# Paned child spans

Paned now computes child width and height through a signed, bounded span
helper.  When margins and borders exceed a collapsed Paned container, child
geometry remains one pixel instead of wrapping an unsigned dimension
subtraction into a huge size.

The source-level regression is `tests/automated/check-paned-child-spans.py`.

---
nav_exclude: true
---
# LabelGadget label initialization rollback

`XmLabelGadget` now checks its initial copies of valid label and menu
accelerator strings.  A failed label copy falls back to the widget name when
that can be converted; otherwise the original valid input remains in place.
A failed accelerator copy likewise retains the original string, avoiding NULL
propagation during construction and subsequent cleanup.

The focused regression is
`tests/automated/check-labelg-init-rollback.py`.

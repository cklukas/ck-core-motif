---
nav_exclude: true
---
# Text secondary-selection wrapper inputs

Text and TextField secondary-selection wrappers now validate the widget,
closure, selection callback, and transfer ID before accessing the display or
forwarding the callback. This makes malformed and teardown-time callbacks
safe at the outer transfer boundary.

Regression coverage: `tests/automated/check-text-secondary-wrapper-inputs.py`.

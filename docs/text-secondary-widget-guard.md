---
nav_exclude: true
---
# Text secondary callback widget guards

Secondary target callbacks now validate the widget and transfer inputs before
querying widget-dependent encoding atoms. Malformed callbacks therefore fail
without entering the Xt/Xm atom lookup path.

Regression coverage: `tests/automated/check-text-secondary-widget-guard.py`.

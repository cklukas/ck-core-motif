---
nav_exclude: true
---
# MWM base-window metric validation

`BaseWindowWidth` and `BaseWindowHeight` return zero for a null
`ClientData` pointer.  These unsigned metric helpers have no error channel;
zero is the safe no-window value.  Valid size calculations remain unchanged.

The source-contract regression is
`tests/automated/check-mwm-base-window-metrics.py`.

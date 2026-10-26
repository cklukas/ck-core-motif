---
nav_exclude: true
---
# MWM title-metric input validation

`TitleTextHeight` and `InitTitleBarHeight` return zero for a null
`ClientData` pointer.  This avoids reading appearance or decoration state from
an absent client; valid title and title-bar metrics are unchanged.

The source-contract regression is
`tests/automated/check-mwm-title-metrics-inputs.py`.

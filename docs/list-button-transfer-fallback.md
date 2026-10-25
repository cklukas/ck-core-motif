---
nav_exclude: true
---
# List button-transfer fallback

List button-transfer actions now ignore invalid `enableBtn1Transfer` resource
values safely. The documented `XmOFF`, `XmBUTTON2_TRANSFER`, and
`XmBUTTON2_ADJUST` values retain their existing behavior; only malformed values
avoid an assertion-driven process termination.

The regression check is
`tests/automated/check-list-button-transfer-fallback.py`.

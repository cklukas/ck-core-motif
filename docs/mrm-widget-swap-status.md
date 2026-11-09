---
nav_exclude: true
---
# Mrm byte-swapped widget conversion status

The hierarchy, indexed-resource, and RID widget readers now return the status
from `Urm__SwapRGMWidgetRecord`. Previously each caller discarded a failed
byte-swap conversion and reported `MrmSUCCESS`, leaving a malformed
cross-endian widget record available to later code. Successful native and
byte-swapped widget reads retain their existing behavior. On a failed in-place
conversion, the readers now clear the widget validation tag before returning
the error, preventing a later retry from accepting a partially swapped record.

The focused source regression is
`tests/automated/check-mrm-widget-swap-status.py`.

---
nav_exclude: true
---

# DataField drop-transfer allocation

`df_HandleDrop` now handles failure to allocate its drop-transfer request
record by starting a failure transfer with zero transfers. The callback state
is never dereferenced or registered after allocation failure, and the drop
protocol receives an explicit failure result.

The focused source contract is checked by
`tests/automated/check-datafield-drop-transfer.py`.

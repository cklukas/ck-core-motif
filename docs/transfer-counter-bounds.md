---
nav_exclude: true
---
# Transfer-counter bounds

Transfer contexts keep request totals in signed `int` fields for ABI
compatibility.  `XmTransferValue` now refuses a new request when either
`count` or `outstanding` has reached `INT_MAX`, before allocating or linking a
transfer block.  This prevents signed wraparound from making a live transfer
appear complete or leaving cleanup counters negative.

The focused regression is
`tests/automated/check-transfer-counter-bounds.py`.

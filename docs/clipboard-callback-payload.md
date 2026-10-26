---
nav_exclude: true
---
# Deferred clipboard callback payloads

Deferred clipboard callbacks convert element counts to byte lengths before
writing the snapshot item. The callback now validates the 8/16/32-bit format,
checks multiplication overflow, and rejects a missing value for non-empty
payloads before copying.

The focused regression is
`tests/automated/check-clipboard-callback-payload.py`.

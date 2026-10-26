---
nav_exclude: true
---
# Deferred clipboard callback inputs

Deferred clipboard callbacks can arrive through cancellation and destruction
paths as well as normal data delivery.  `ClipboardCallback` now rejects a
missing reason, and rejects a missing target for non-delete callbacks before
dereferencing either pointer.  It also rejects duplicate or already-completed
data IDs and removes the hash entry using the data-ID value, matching the key
used when the request was registered.  This prevents callback-time crashes,
counter underflow, and stale snapshot entries.

The focused regression is
`tests/automated/check-transfer-callback-inputs.py`.

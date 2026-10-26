---
nav_exclude: true
---

# Transfer-id list locking

Active transfer IDs and reusable transfer IDs share process-global linked
lists. `FreeTransferID` now unlinks an ID and adds it to the free list while
holding `_XmProcessLock`, matching `GetTransferID` and preventing concurrent
completion callbacks from corrupting the list. A null ID is ignored safely.

The regression check is
`tests/automated/check-transfer-id-locking.py`.

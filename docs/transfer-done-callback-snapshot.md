---
nav_exclude: true
---
# Transfer completion callback snapshot

Transfer completion now copies the callback list and operation while holding
the transfer process lock, then invokes the private snapshot after unlocking.
Concurrent cleanup or reentrant completion can no longer reclaim the array
while it is being traversed.

Regression coverage: `tests/automated/check-transfer-done-callback-snapshot.py`.

---
nav_exclude: true
---

# Clipboard lock allocation

The Motif clipboard lock path allocates a small lock record when no lock
property exists. Allocation failure is now reported as `ClipboardFail` after
releasing the application lock, before the record is initialized. Existing
clipboard ownership and lock-level behavior is unchanged for successful
allocations.

The focused regression check is
`tests/automated/check-clipboard-lock-allocation.py`.

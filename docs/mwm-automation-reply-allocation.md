---
nav_exclude: true
---

# MWM automation reply allocation guards

All `GetAutomationData` operation cases now use one checked allocation path.
The helper rejects size multiplication overflow and allocation failure before
the operation-specific packer runs, returning an empty reply while preserving
the existing wire layout for successful requests.

The focused regression check is
`tests/automated/check-mwm-automation-reply-allocation.py`.

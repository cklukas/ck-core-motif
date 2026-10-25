---
nav_exclude: true
---

# MWM client workspace-list allocation

When WSM support is enabled, each managed client receives a workspace-data
array sized from the screen resource configuration. The allocation now rejects
negative counts and products that cannot fit Xt's `Cardinal` size before the
multiplication is evaluated.

The existing allocation-failure warning and `NULL` return contract are
unchanged. The guard is covered by
`tests/automated/check-mwm-client-workspace-list-allocation.py`.

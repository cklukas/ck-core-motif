---
nav_exclude: true
---
# Mrm literal-fetch context setup

All public literal fetchers now propagate resource-context allocation failure
before attempting to load or free through that context, and release their MRM
locks on that early return. Icon literal loading validates its context-list
output, propagates pointer-list allocation failure, and reuses a supplied list
instead of replacing it. Valid literal, icon, bitmap, and color fetches retain
their existing conversion and ownership behavior.

The source-level regression is
`tests/automated/check-mrm-fetch-literal-context-setup.py`.

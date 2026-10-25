---
nav_exclude: true
---
# Render-table token allocation

Render-table property parsing reuses a token record while creating temporary
strings. Every token length is checked against Xt's `Cardinal` width before
allocation; on sizing or allocation failure the token is marked EOF and the
existing parser termination/error path is used. Successful render-table
parsing is unchanged.

`tests/automated/check-render-token-allocation.py` protects this contract.

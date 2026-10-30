---
nav_exclude: true
---
# Container begin-toggle input guards

The Container begin-toggle action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button traversal
and toggle selection from malformed programmatic action calls while leaving
normal pointer-driven behavior unchanged.

The source contract is checked by
`tests/automated/check-container-begin-toggle-input-guards.py`.

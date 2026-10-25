---
nav_exclude: true
---
# Mrm literal context initialization

Mrm literal loading now checks the result of `UrmGetResourceContext` before
reading or storing data through the returned context.  Allocation failure is
reported to the caller, while successful literal loading keeps the existing
resource and callback behavior.

The source-level regression is
`tests/automated/check-mrm-literal-context-init.py`.

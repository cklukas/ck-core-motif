---
nav_exclude: true
---
# Manager class-extension allocation

Manager class initialization now checks the inherited composite extension and
the manager extension allocation before copying or initializing either
record.  Under allocation pressure, class initialization returns without
dereferencing a null extension instead of crashing the process during widget
class setup.  The normal inheritance and resource behavior is unchanged when
the allocations succeed.

The focused source-level regression is
`tests/automated/check-manager-class-allocation.py`.

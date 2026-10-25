---
nav_exclude: true
---
# ScrolledWindow auto-drag allocation

Scrollbar auto-scrolling begins a drop-site update before creating its timer
closure.  The closure allocation is now checked; on failure, the update is
balanced and no timer or dangling closure is published.  Normal auto-scroll
behavior is unchanged when allocation succeeds.

`tests/automated/check-scrolled-window-drag-allocation.py` protects this
contract.

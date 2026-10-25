---
nav_exclude: true
---
# Hierarchy top-node allocation

Hierarchy widgets create a top constraint node during initialization and then
initialize its state and parent pointers. The initializer now checks the Xt
allocation before those writes and returns cleanly when memory is unavailable.

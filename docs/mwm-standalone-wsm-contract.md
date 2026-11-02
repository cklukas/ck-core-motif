---
nav_exclude: true
---
# MWM standalone workspace contract

The standalone `MWM_WSM` workspace implementation sends workspace-manager
messages and removes client commands when an owner changes. It therefore needs
the same per-screen UTM shell and command-tree state as the legacy command
protocol. Those members, their initialization, and supporting declarations are
now enabled for `MWM_WSM` builds as well as the historical configurations.

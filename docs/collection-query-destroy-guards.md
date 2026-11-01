---
nav_exclude: true
---

# Collection-widget query teardown guards

Row Actions, Command Palette, Status Bar, Toolbar, Segmented Control, and
Tree View now make their public collection queries no-ops while destruction is
in progress. Counts return zero, state queries return false or their existing
empty sentinel, and pointer-returning queries return `NULL` instead of touching
child or item storage that Xt may already be releasing.

Live-widget APIs and data ownership are unchanged.

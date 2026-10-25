---
nav_exclude: true
---
# TearOff child-count bounds

TearOff menu handler installation/removal and submenu redraw walk Xt composite
child arrays whose count is a `Cardinal`. Those loops now use `Cardinal`
indices, preserving handler and redraw behavior while avoiding signed-index
wraparound for large menu child lists. The separate excluded-pane list keeps
its existing signed index because its legacy count is explicitly a `short`.

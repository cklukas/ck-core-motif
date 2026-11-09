---
nav_exclude: true
---

# Navigation mutator teardown guards

TreeView top-row selection, BreadcrumbBar current-index selection, and the
Sidebar toggle entry point now reject widgets once Xt destruction begins.
This keeps late key, pointer, and application calls from recalculating layout,
invalidating a drawable, or reading navigation state during teardown.

`tests/automated/check-navigation-mutator-destroy-guards.py` checks the three
public entry points.

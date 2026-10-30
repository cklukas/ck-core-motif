---
nav_exclude: true
---

# TabStack menu action guards

The TabStack menu action now validates its invoking widget, event, parent, and
TabStack ancestor before translating coordinates or updating tear-off state.
Malformed action invocations return safely; valid menu positioning remains
unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-menu-action-guards.py`.

---
nav_exclude: true
---

# Navigability trait guards

Manager, Primitive, and Gadget navigability callbacks now validate their
widget class before reading traversal and sensitivity state. Invalid inputs
return `XmNOT_NAVIGABLE`; valid widgets retain the existing tab/control
navigability decisions.

The focused source-contract check is
`python3 tests/automated/check-navigability-trait-guards.py`.

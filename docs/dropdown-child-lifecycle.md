---
nav_exclude: true
---
# DropDown child lifecycle

DropDown keeps its list, text field, arrow, and optional label as internal
children.  Initialization, `GetValues`, and later `SetValues` operations now
check those handles before registering handlers, forwarding resources,
changing visual metrics, or managing labels.

This preserves normal behavior for a complete DropDown while making resource
updates safe after allocation pressure leaves one of the optional children
unavailable.

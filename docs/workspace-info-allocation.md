---
nav_exclude: true
---
# Workspace information property allocation

Mwm now checks both temporary buffers used to publish a workspace information
property.  If either allocation fails, any successful sibling allocation is
released and the property update returns without dereferencing invalid state.
The normal property encoding is unchanged.

`tests/automated/check-workspace-info-allocation.py` protects this path.

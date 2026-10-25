---
nav_exclude: true
---
# Mwm workspace-info title conversion

`SetWorkspaceInfoProperty` converts the workspace title before passing the
string vector to Xlib.  A missing or malformed `XmString` can make that
conversion return `NULL`; the property update now logs the failure, releases
its temporary buffers, and returns before Xlib sees a null list entry.  This
preserves the existing property format while avoiding a crash in the Xmb
conversion path.

`tests/automated/check-workspace-info-title.py` protects this failure path.

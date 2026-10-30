---
nav_exclude: true
---
# Tear-off shell allocation

Tear-off menu creation now checks the transient shell returned by
`XtCreatePopupShell` before setting its title, interning window-manager atoms,
or registering callbacks.  Allocation failure returns cleanly and leaves the
original menu hierarchy intact.

---
nav_exclude: true
---

# MWM CDE workspace-manager window lookup

When MWM is built with its optional Workspace Services Manager (WSM) support,
it obtains the manager window through CDE's private WSM interface:

```c
_DtGetMwmWindow(display, root, &wm_window);
```

That is the CDE-declared helper for locating the Motif window-manager window.
It returns the Xlib property-query status and writes the window only on a
successful lookup. MWM preserves the established startup behavior by retaining
that return convention; the result is subsequently used by the existing
session-manager protocol path.

The focused source regression is
`tests/automated/check-mwm-cde-wsm-window-helper.py`.

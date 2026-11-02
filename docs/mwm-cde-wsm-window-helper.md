---
nav_exclude: true
---

# MWM CDE session-manager window lookup

When MWM is built with its optional Workspace Services Manager (WSM) support,
it obtains the CDE **session-manager** window before it sends the
`_DT_SM_WM_PROTOCOL` readiness and acknowledgement messages:

```c
_DtGetSmWindow(display, root, &sm_window);
```

The distinction matters: CDE's `_DtGetMwmWindow` finds the **window-manager**
window and is used by a session manager when it needs to contact MWM. MWM's
`wmGD.dtSmWindow` is the inverse endpoint; it is used to send session-manager
protocol messages. CDE's own `dtwm/WmInitWs.c` uses `_DtGetSmWindow` for this
field and includes `Dt/SessionM.h` for its declared prototype. MWM does the
same when optional WSM support is enabled.

Both helpers return the Xlib property-query status and write their output only
on a successful lookup. MWM retains that established optional-WSM startup
behavior; it does not redirect acknowledgements to its own manager window when
no session manager is running.

The focused source regression is
`tests/automated/check-mwm-cde-wsm-window-helper.py`.

# MWM confirmation-window geometry

Confirmation windows are internally managed clients, but their X window can
still disappear during setup. `GetWmClientInfo` now requires
`XGetGeometry` to succeed before recording the client dimensions and border.
Failure returns `NULL` with a warning, avoiding partially initialized state.

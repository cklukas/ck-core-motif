# MWM workspace helper-window creation failure

The hidden per-screen workspace helper window is an implementation detail used
for Motif workspace properties. `SetupWmWorkspaceWindows` now checks the
`XCreateWindow` result before mapping the helper or publishing its ID in
Motif state. A resource-allocation failure therefore leaves that optional
helper unavailable without issuing requests against `None`.

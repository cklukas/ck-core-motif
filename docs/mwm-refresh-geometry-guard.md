# MWM refresh geometry guard

The temporary window used by `F_Refresh_Win` receives unsigned X11 dimensions,
while client and maximized geometry is stored in signed `int` fields.  The
refresh action now skips the request when either dimension is non-positive,
avoiding a negative-to-unsigned conversion that could request a huge window
during a malformed-hints or teardown transition.

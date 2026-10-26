# MWM unmanaged-screen warning formatting

The `F_Screen` unmanaged-screen warning uses a fixed format string.  Both
`snprintf` calls now pass that literal directly, preserving compiler format
checking and the existing warning text while retaining the allocation guard.

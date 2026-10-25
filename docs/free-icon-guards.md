---
nav_exclude: true
---
# FreeIcon teardown guards

`FreeIcon` can run while a client is being withdrawn or while workspace
membership is being torn down.  The cleanup routine now returns when the
client or screen state is unavailable, and checks that the workspace list has
at least one entry before examining its first icon-box pointer.  This avoids a
null dereference and an out-of-bounds read while retaining the existing icon
window and placement cleanup for fully initialized clients.

# Mrm pointer-list append cleanup

`UrmPlistAppendString` allocates a copy before growing the pointer list. If the
list resize or append fails, the newly allocated copy is now freed before the
failure is returned. Successful appends retain ownership in the list as
before.

# DeleteEntryFromList guards

Window-list removal can occur after workspace teardown has begun.  The
low-level unlink helper now validates the workspace, screen, and list-entry
pointers before updating sibling links or screen list heads.  Normal unlinking
is unchanged for valid list state.

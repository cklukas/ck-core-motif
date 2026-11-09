# Container cell-grid reset cleanup

`XmContainer` subclasses may omit the optional `remove_item` class hook.  A
spatial reset still owns its prior occupancy grid in that case.  Reset now
invokes the hook only when supplied, then always releases and clears the old
grid before rebuilding or returning for a non-spatial layout.  This prevents a
leak and stale grid state without changing hook behavior for existing classes.

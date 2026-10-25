# Preserve list height state while adding items

`APIAddItems` passes the previous maximum item height to `SetNewSize` so a
list can refresh its visible-item geometry when an insertion changes the
height extent.  The local variable was declared without an initializer,
which left the comparison undefined on the normal successful insertion path.

The value is now captured from `MaxItemHeight` before the insertion.  This
preserves the existing resize behavior while making the state comparison
deterministic on all supported architectures.

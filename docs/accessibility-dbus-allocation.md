# Accessibility D-Bus state allocation

The AT-SPI D-Bus transport keeps per-node memo records for cache entries,
text snapshots, and state-change notifications.  All paths now treat a failed
memo-record allocation as a recoverable transport failure instead of
dereferencing a null record.  Cache publication is skipped and the current
notification is abandoned while the native accessibility tree remains intact.

This keeps allocation pressure from crashing the UI process or the D-Bus
bridge, while allowing later notifications to retry record creation.

Initial tree seeding follows the same rule: a node whose memo record cannot be
created is skipped and the remaining tree continues to be announced.

---
nav_exclude: true
---
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

Method-return message construction is also checked before initializing an
iterator, so D-Bus memory pressure produces a handled request rather than a
null-message crash.

AT-SPI state, window, cache, hierarchy, and property-change signals now use
the same fail-closed rule: a missing signal is skipped without touching an
uninitialized iterator or changing the native widget state.

Session-bus address lookup and registry embedding also handle failed method
message construction, releasing the partially initialized transport cleanly.

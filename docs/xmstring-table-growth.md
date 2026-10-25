---
nav_exclude: true
---
# XmString table growth bounds

The string-to-`XmString` resource converter grows its pointer table as it
parses comma-separated entries.  Capacity doubling now checks for `int`
overflow first and passes the checked initial, growth, and terminating table
sizes explicitly as Xt `Cardinal` byte counts.  If the limit is reached,
already-created strings and the table are released and conversion fails
instead of wrapping the capacity and writing past the allocation.

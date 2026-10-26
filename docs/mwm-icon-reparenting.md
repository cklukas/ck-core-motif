# MWM client icon reparenting

Client-provided icon windows can be destroyed independently of the managed
client. `ReparentIconWindow` now checks the icon and destination frame IDs and
requires `XGetGeometry` to succeed before using the returned dimensions. This
prevents stale external XIDs from causing invalid configure/reparent requests
or uninitialized placement calculations.

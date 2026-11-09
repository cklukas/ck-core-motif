---
nav_exclude: true
---
# MWM client-list growth

Session-management discovery validates its public output pointers and skips
empty screen-list entries before examining a client. Growth rejects a null
client, validates the count, byte multiplication, and Xt's Cardinal width, and
leaves the existing list untouched if XtRealloc fails. Valid additions retain
their order and count behavior.

`tests/automated/check-mwm-client-list-growth.py` protects this contract.

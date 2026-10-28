---
nav_exclude: true
---
# MWM session-resource integers

Restored MWM session state contains coordinates, dimensions, and icon
positions supplied by a resource database. These values are now parsed as
complete, range-checked integers. Malformed or overflowing values are ignored
without setting the corresponding restore flag, so a bad session file cannot
silently reposition or resize a client to zero.

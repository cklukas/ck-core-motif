---
nav_exclude: true
---
# Dialog constructor argument counts

`XmeCreateClassDialog` adds one shell argument to the caller's argument list.
It now rejects the maximum `Cardinal` count before the addition and allocation
can wrap. Ordinary dialog construction is unchanged.

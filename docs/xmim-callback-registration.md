---
nav_exclude: true
---
# Input-method callback registration guards

Real XIM callbacks are now installed only for an existing widget reference and
an allocated callback array. Stale widget callbacks and allocation failures
leave the input-method state unchanged.

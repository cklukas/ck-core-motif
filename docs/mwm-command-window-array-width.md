---
nav_exclude: true
---

# MWM command window-array width

WSM Include, Enable, Disable, Rename, and Remove commands receive window
counts from the protocol. Each handler now checks both multiplication overflow
and Xt's `Cardinal` allocation width before creating its temporary window-ID
array. Oversized protocol input follows the existing diagnostic return path.

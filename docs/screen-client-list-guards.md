---
nav_exclude: true
---
# Screen client-list update guards

The `_MWM_CLIENT_LIST` notification path now handles a missing screen, detached
client entries, and allocation failure without writing through a null buffer.
A failed update leaves the existing property untouched; valid entries are
reported exactly as before.

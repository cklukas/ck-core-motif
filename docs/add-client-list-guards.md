---
nav_exclude: true
---
# AddClientToList argument guards

Client management can observe a workspace lookup failure while a workspace is
being removed.  `AddClientToList` now returns without touching list or screen
state when either its workspace or client argument is missing.  Valid calls
follow the existing stacking and modal-window behavior unchanged.

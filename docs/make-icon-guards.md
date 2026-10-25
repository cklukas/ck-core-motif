---
nav_exclude: true
---
# MakeIcon argument guards

Icon creation can be requested while workspace membership is being removed.
`MakeIcon` now rejects a missing workspace or client before touching icon
geometry or workspace-client data.  Existing callers already treat a false
return as an icon creation failure and unwind management, so normal behavior
and compatibility are unchanged.

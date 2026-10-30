---
nav_exclude: true
---
# XmList conversion callback guards

`ListConvert` now validates its widget and conversion callback pointers before
querying the encoding atom or reading drag state. Invalid callback dispatch
returns safely; normal list conversion keeps the same target and payload
behavior.

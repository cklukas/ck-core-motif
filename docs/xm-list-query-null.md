---
nav_exclude: true
---
# XmList query output validation

`XmListGetMatchPos` and `XmListGetSelectedPos` now validate their output
pointers before initialization and reject null widgets. Match queries also reject
a null item before entering the app context. Invalid calls return `False` with
provided outputs initialized to an empty result; valid list queries retain their
existing allocation and result behavior.

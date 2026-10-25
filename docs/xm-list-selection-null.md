# XmList selection helper null-safety

`XmListUpdateSelectedList` and `XmListPosSelected` now reject null widgets
before app-context locking and list-state access. Valid selection regeneration
and position queries retain their existing behavior.

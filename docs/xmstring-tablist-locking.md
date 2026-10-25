# XmString tab-list proposal locking

`XmStringTableProposeTablist` now delegates synchronization to tab-list,
conversion, and rendering helpers instead of holding the process lock across
them. Cleanup behavior on allocation and invalid-input failures is unchanged.

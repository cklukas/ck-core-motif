# XmString table conversion locking

`XmStringToXmStringTable` now delegates synchronization to its context and
string helpers instead of holding the process lock across them. Its table
allocation failure path also frees the active context before returning.

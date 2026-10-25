# Representation-type registration allocation

`XmRepTypeRegister` grows the process-wide dynamic representation-type table
before filling the new record. The resize result is now checked before the
global pointer is replaced or the new record is accessed. An allocation
failure returns `XmREP_TYPE_INVALID` while preserving the existing registered
types and converter state. `XmRepTypeGetRegistered` likewise returns `NULL`
when its result list cannot be allocated, without touching the process-wide
registry.

`XmRepTypeGetRecord` also checks its individual record allocation and returns
`NULL` without leaving the process lock held when memory is unavailable.

This keeps the public registration API and successful registration behavior
unchanged while preventing a null-table dereference under memory pressure.

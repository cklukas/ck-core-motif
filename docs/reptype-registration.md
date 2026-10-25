# Representation-type registration allocation

`XmRepTypeRegister` grows the process-wide dynamic representation-type table
before filling the new record. The resize result is now checked before the
global pointer is replaced or the new record is accessed. An allocation
failure returns `XmREP_TYPE_INVALID` while preserving the existing registered
types and converter state.

This keeps the public registration API and successful registration behavior
unchanged while preventing a null-table dereference under memory pressure.

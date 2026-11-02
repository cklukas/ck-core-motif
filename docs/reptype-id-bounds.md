---
nav_exclude: true
---
# Representation-type identifier bounds

`XmRepTypeId` reserves `XmREP_TYPE_INVALID` as its invalid sentinel. Dynamic
registration now refuses a record whose identifier would reach that sentinel,
and wide-prototype public entry points reject negative or out-of-range integer
identifiers before narrowing them to `XmRepTypeId`. Wide-prototype registration
also rejects value counts that cannot fit the public `unsigned char` field.

This prevents an invalid identifier from aliasing a valid registry record while
preserving all existing valid registration and lookup behavior. The focused
source regression is `tests/automated/check-reptype-id-inputs.py`; the native
RepType consumer also verifies that the reserved sentinel has no record, name
list, or valid value.

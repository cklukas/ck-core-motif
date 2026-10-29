---
nav_exclude: true
---
# TabStack child allocation

`XmTabStack` creates an internal `XmTabBox` that owns tab selection and
geometry. If Xt cannot allocate that child, initialization now skips callback
registration, geometry returns a stable answer, and later layout/resource
updates avoid dereferencing the missing TabBox. Managed tab children remain
owned by the stack and can be recovered by a later resource or lifecycle
operation without changing normal TabBox behavior.

The contract is protected by
`tests/automated/check-tabstack-child-allocation.py`.

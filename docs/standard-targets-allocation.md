---
nav_exclude: true
---
# Standard transfer target allocation

`XmeStandardTargets` returns the built-in target atoms with capacity reserved
for the caller's private targets. It now computes that final capacity once and
checks the allocation before filling the atom list. Allocation failure unlocks
the application context, reports zero built-in targets, and returns `NULL`.

Successful callers receive the same atom ordering and spare capacity; the
change removes an unnecessary resize and prevents a null write under memory
pressure.

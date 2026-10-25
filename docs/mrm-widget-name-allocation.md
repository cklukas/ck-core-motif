# Mrm widget-name allocation failure

`UrmSetWidgetInstance` now checks the temporary name buffer returned by
`ALLOCATE_LOCAL` before writing the lookup prefix. Allocation failure is
reported through the existing Mrm error path instead of causing a null-pointer
dereference.

The source-level regression is
`tests/automated/check-mrm-widget-name-allocation.py`.

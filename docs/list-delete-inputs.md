---
nav_exclude: true
---
# List deletion input validation

`XmListDeleteItems` now rejects a null widget, null item array, or non-positive
item count before reading widget state or entering the application context.
Valid deletion calls retain their existing behavior.

`tests/automated/check-list-delete-inputs.py` protects this contract.

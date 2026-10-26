---
nav_exclude: true
---
# TextField transfer-action allocation

Button-drag transfer setup now allocates the action record and event as a
transaction.  If either allocation fails, partial state is released and the
original action is invoked instead of copying through a null event pointer.

Regression coverage: `tests/automated/check-textfield-transfer-action.py`.

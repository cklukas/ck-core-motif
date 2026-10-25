---
nav_exclude: true
---
# SpinBox child-name bounds

`XmSimpleSpinBox` creates its text-field child by appending `_TF` to the
parent widget name.  The old path added a fixed amount to `strlen` and then
used `sprintf`, allowing allocation arithmetic to wrap for an unusually long
name.

The child name is now measured with `snprintf`, allocated with room for the
exact result and terminator, and formatted with a bounded second pass.  The
resource and child naming contract is unchanged.

`tests/automated/check-spinbox-name-bounds.py` protects this behavior.

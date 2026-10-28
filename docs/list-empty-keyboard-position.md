---
nav_exclude: true
---
# Empty List keyboard position

An empty List has no current keyboard item, even if its item-array pointer has
been allocated. The getter now returns zero for that state, and the setter
rejects all positions before attempting highlight or visibility work. This
prevents a phantom position and avoids a `-1` internal index.

Regression coverage: `tests/automated/check-list-empty-keyboard-position.py`.

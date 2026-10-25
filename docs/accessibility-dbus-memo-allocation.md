# Accessibility D-Bus memo allocation handling

The accessibility D-Bus transport memo table now reports allocation failure
from table resizing and record creation.  Callers leave the memo table
unchanged and return no record when growth cannot be completed, avoiding null
table or record dereferences while preserving existing live-node memo behavior.

The regression check is `tests/automated/check-accessibility-dbus-memo-allocation.py`.

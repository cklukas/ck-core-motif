# Workspace-list allocation

Workspace-list parsing now checks its initial array of parsed-name pointers
before filling it.  Allocation failure follows the same warning and controlled
window-manager exit path as later workspace-data allocation failures.

`tests/automated/check-workspace-list-allocation.py` protects this startup
path.

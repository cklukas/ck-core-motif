# WSM WM_WINDOWS property allocation failure

WSM `WM_WINDOWS` request unpacking now checks its window-property array before
copying wire properties. A failed allocation clears the property count and
returns before writing through a null pointer.

`tests/automated/check-wsm-wm-windows-oom.py` protects this failure path.

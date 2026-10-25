# WSM config-format reply allocation failure

WSM configuration-format reply unpacking now returns cleanly when its screen
metadata record cannot be created. This avoids dereferencing a null screen
record after display-cache allocation failure; normal configuration-format
replies continue through the existing attribute-list path.

`tests/automated/check-wsm-config-reply-oom.py` protects this boundary.

---
nav_exclude: true
---
# Search-box placeholder lifecycle

`XmSearchBox` owns its copied placeholder `XmString`.  During `XtSetValues`,
the old owned copy is now released only after a replacement is created; if
copying fails, the previous placeholder remains in use.  This prevents leaks
from repeated placeholder updates without changing valid resource behavior.

`tests/automated/check-searchbox-placeholder-lifecycle.py` protects this
ownership and allocation-failure path.

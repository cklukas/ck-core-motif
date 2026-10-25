---
nav_exclude: true
---
# Compound-text direction-stack growth

Resource encoding direction control now grows its stack through a checked
helper.  A failed `XtRealloc` leaves the current stack and depth unchanged and
causes CSI processing to report failure instead of writing through a null or
partially published stack.

`tests/automated/check-resencod-direction-stack.py` protects this path.

Both public and resource-converter context constructors now check their
context and initial direction-stack allocations and release partial state on
failure.

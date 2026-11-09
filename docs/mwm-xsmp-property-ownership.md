---
nav_exclude: true
---

# MWM XSMP property-buffer ownership

MWM obtains proxy-client command and machine strings through Xlib property
queries.  Those buffers are Xlib-owned and must be released with `XFree`, not
the C allocator.  The proxy cleanup is now centralized so both the matching
and session-save paths use the correct allocator, including an early failure
while formatting the generated client identifier.

`tests/automated/check-mwm-xsmp-property-release.py` protects this ownership
contract.

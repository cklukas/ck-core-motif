---
nav_exclude: true
---

# WSM target conversion allocation failure

Target/type conversion now checks whether display metadata was available.  If
the display-info allocation fails, `_WSMTargetToReqType` returns
`WSM_UNKNOWN` and `_WSMReqTypeToTarget` returns `None` instead of dereferencing
a null metadata pointer.  Successful conversions and protocol atoms are
unchanged.

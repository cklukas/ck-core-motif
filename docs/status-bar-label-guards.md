---
nav_exclude: true
---

# Status-bar label allocation guards

`XmStatusBar` now handles failed text conversion and allocation while composing
plain and progress-item labels. Progress text keeps its existing formatting;
only invalid or unrepresentable allocations are rejected.

The focused regression check is
`tests/automated/check-status-bar-label-guards.py`.

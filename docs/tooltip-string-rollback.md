---
nav_exclude: true
---
# ToolTip string rollback

`XmSetToolTipString` now copies a non-NULL replacement before releasing the
current tooltip.  Allocation failure leaves the existing tooltip intact;
passing NULL still intentionally clears it.

The focused regression is `tests/automated/check-tooltip-string-rollback.py`.

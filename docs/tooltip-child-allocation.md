---
nav_exclude: true
---
# ToolTip child allocation

Legacy ToolTip lazily creates a popup shell, label, and slide context. Creation
failures and timer callbacks now tolerate missing children and popup parents;
failed tooltip presentation returns cleanly instead of dereferencing a null
label or slider during posting and unposting.

The contract is protected by
`tests/automated/check-tooltip-child-failure-guards.py`.

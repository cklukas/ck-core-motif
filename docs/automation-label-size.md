# Mwm automation menu-label sizing

Mwm’s automation replies include menu labels in variable-length buffers. The
menu-item check-info path now sizes the complete label, including its first
character and terminating byte, matching the other menu reply paths. This
preserves the wire format while preventing underallocation for nonempty
labels.

`tests/automated/check-automation-label-size.py` guards the sizing expression.

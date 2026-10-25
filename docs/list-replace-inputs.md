---
nav_exclude: true
---
# List replacement input validation

List replacement APIs now reject null widgets before toolkit access. Shared
replacement paths also reject non-positive counts and null old or new
`XmString` elements before comparing or replacing entries.

`tests/automated/check-list-replace-inputs.py` protects this contract.

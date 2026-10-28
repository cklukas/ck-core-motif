---
nav_exclude: true
---
# PrintShell child failure status

The `XmPrintToFile` child now reports setup failures through the existing
status pipe. Allocation or display-open failures close and remove the partial
file before reporting `XPGetDocError`. The parent also treats a truncated or
interrupted status read as an error rather than consuming an uninitialized
status value.

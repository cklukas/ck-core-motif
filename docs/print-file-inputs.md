---
nav_exclude: true
---
# PrintShell output-file inputs

`XmPrintToFile` now rejects a null display, null path, or empty path before
opening the output. If child creation fails after opening a valid path, the
empty file is removed before returning failure. Valid output paths retain the
existing print workflow and callback behavior.

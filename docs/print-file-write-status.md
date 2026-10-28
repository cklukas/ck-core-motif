---
nav_exclude: true
---
# PrintShell file-output status

`XmPrintToFile` now checks every child-side `fwrite` and the final `fclose`.
When either operation fails, the finish callback reports `XPGetDocError` and
removes the partial output file. Successful X Print completion keeps the
existing file and status behavior.

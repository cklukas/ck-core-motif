---
nav_exclude: true
---
# PrintShell callback setup

`XmPrintToFile` allocates and initializes its parent status callback before
forking the document writer. This prevents callback allocation failure from
returning failure while a child continues writing. Fork failure releases the
callback, closes both pipe ends, and removes the output file.

---
nav_exclude: true
---
# Print setup lifecycle

`XmPrintSetup` now rejects null inputs before walking the video widget's
ancestor chain. It also checks application-shell and print-shell allocation,
destroying the temporary application shell when print-shell creation fails.

Successful print setup retains the existing callback, realization, and mapped
state behavior.

---
nav_exclude: true
---
# VendorShell tooltip-trait allocation

The VendorShell initialization prehook creates a tooltip configuration trait
when the class does not provide one. The trait allocation is now checked before
its default fields are initialized; failure returns from the prehook without a
null dereference.

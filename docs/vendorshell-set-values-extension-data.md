---
nav_exclude: true
---
# VendorShell set-values extension data

VendorShell set-values processing clones extension metadata when an existing
shell extension record is present. The clone allocation is now checked before
copying or pushing the record, so failure leaves the existing extension data
unchanged.

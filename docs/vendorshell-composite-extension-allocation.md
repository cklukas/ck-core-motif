---
nav_exclude: true
---
# VendorShell composite-extension allocation

VendorShell class initialization now checks the inherited composite-extension
lookups and the `XtNew` allocation before copying the superclass record.  A
missing or unallocatable extension causes class initialization to return
without dereferencing null state; successful initialization retains the
existing inheritance and trait behavior.

The focused source-level regression is
`tests/automated/check-vendorshell-composite-extension-allocation.py`.

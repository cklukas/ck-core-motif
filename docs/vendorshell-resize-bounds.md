# VendorShell resize bounds

VendorShell resize now returns safely when its shell extension data is absent
or incomplete.  It also computes the child height with signed operands and
clamps it to zero when input-method height exceeds the shell height, avoiding a
wrapped oversized child configuration.

The focused regression is
`tests/automated/check-vendorshell-resize-bounds.py`.

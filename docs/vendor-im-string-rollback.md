---
nav_exclude: true
---
# Vendor input-method string rollback

`XmVendorShellExt` now copies changed input-method and preedit-type strings
before releasing their current values.  Allocation failure preserves the
previous configuration, while assigning NULL still clears a value explicitly.

The focused source regression is
`tests/automated/check-vendor-im-string-rollback.py`.

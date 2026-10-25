---
nav_exclude: true
---
# XmString creation tag-cache failures

`_XmStringNCreate` now verifies that the tag-cache index resolves to the
requested tag and that non-optimized tag caching succeeds. Cache allocation
failure no longer silently substitutes the default charset or returns a
segment with a null tag.

The regression check is
`tests/automated/check-xmstring-create-tag-cache.py`.

---
nav_exclude: true
---
# MWM workspace-title initialization rollback

When MWM initializes workspace resources, it now publishes the copied title
only after `XmStringCopy` succeeds.  A failed copy leaves the configured
workspace title intact instead of replacing it with NULL.  The change is
limited to ownership handling and preserves the existing MWM resource ABI.

The focused regression is
`tests/automated/check-mwm-workspace-title-init-rollback.py`.

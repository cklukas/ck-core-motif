---
nav_exclude: true
---
# Tab-list modification rollback

`XmTabbedStackListModify` now copies a replacement label before releasing a
copied label.  A failed allocation leaves both the existing label and its
value mode unchanged, including when a shared label is being changed to copy
ownership.  NULL remains a supported explicit clear value.

The focused source regression is
`tests/automated/check-tablist-modify-rollback.py`.

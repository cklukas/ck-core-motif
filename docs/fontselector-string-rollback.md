---
nav_exclude: true
---
# FontSelector string rollback

FontSelector now copies each changed label string before releasing its current
value. Allocation failure preserves the old selector label and skips the
corresponding widget refresh; NULL remains an explicit clear operation.

The focused source regression is
`tests/automated/check-fontselector-string-rollback.py`.

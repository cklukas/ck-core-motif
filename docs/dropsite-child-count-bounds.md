---
nav_exclude: true
---
# DropSite descendant traversal

DropSite descendant discovery now uses a `Cardinal` index for composite child
counts. This matches Xt's count field and avoids signed-index wraparound while
searching large widget trees for drag-and-drop targets.

The focused source regression is
`tests/automated/check-dropsite-child-count-bounds.py`.

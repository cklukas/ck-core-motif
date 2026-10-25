# Notebook child-count traversal

Notebook redisplay, join-side updates, page-number assignment, active-child
tracking, and child lookup use `Cardinal` indices for the composite child
count. The page-number helper accepts the same full-width index, avoiding
signed-index wraparound when a valid notebook has many children.

The focused source regression is
`tests/automated/check-notebook-child-count-bounds.py`.

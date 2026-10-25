# Manager child-count traversal

`CheckRemoveMotionHandlers` now uses a `Cardinal` index while scanning the
manager's composite children. This matches Xt's child-count field and avoids
signed-index wraparound for large, valid manager child lists.

The focused source regression is
`tests/automated/check-manager-child-count-bounds.py`.

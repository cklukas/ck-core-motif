# ColorSelector RGB-file rollback

ColorSelector now copies a changed RGB-file path before releasing the current
path. Allocation failure preserves the current file and color list instead of
leaving an unusable NULL path; a successful changed path still rebuilds the
list as before.

The focused source regression is
`tests/automated/check-color-selector-rgb-file-rollback.py`.

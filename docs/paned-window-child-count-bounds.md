# PanedWindow child-count traversal

The maintained `XmPanedWindow` implementation now uses full-width `Cardinal`
indices when updating pane positions, clipping sash regions, and rebuilding
managed-child lists. This matches Xt's composite child-count field and avoids
signed-index wraparound in large PanedWindow hierarchies.

The focused source regression is
`tests/automated/check-paned-window-child-count-bounds.py`.

# XmPicture repeat node-map bounds

Picture repeat expansion checks the current node count before allocating its
per-copy node map. An empty or unrepresentable map leaves the existing graph
unchanged and returns the current end node; ordinary repeat expansion keeps its
existing behavior.

The source-level regression is
`tests/automated/check-picture-copy-bounds.py`.

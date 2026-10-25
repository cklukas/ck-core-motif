# Tree child and argument count bounds

Tree constraint updates and layout traversal use `Cardinal` indices for Xt
argument counts and composite-child counts. This keeps the internal loops
within the full range of their public/count fields instead of allowing a
signed `int` index to wrap on a large, valid widget tree.

The focused source regression is
`tests/automated/check-tree-count-bounds.py`.

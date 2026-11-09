# Tree connector rendering GC failure

Tree endpoint-gutter connector drawing uses a private graphics context so it
does not alter the shared connector GC's clipping state. If that temporary GC
cannot be allocated, Tree now skips only that connector extension and leaves
the normal parent-window connector rendering intact. This avoids invalid X
requests without changing Tree resources or layout behavior.

The focused source-contract regression is
`tests/automated/check-tree-connector-gc.py`.

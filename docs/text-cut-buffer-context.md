# Text cut-buffer context allocation

Per-screen cut-buffer setup now checks its context-record allocation before
initializing fields or registering the destroy callback.  Allocation failure
returns without changing X11 state, allowing a later setup attempt to retry
safely.

The focused source contract is checked by
`tests/automated/check-text-cut-buffer-context.py`.

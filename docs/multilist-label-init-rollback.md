# MultiList label initialization rollback

MultiList now checks its initial title, titleString, and find-label copies.
Failed copies fall back to empty or default find labels before child widgets
are created, preventing NULL label propagation during construction.

The focused regression is
`tests/automated/check-multilist-label-init-rollback.py`.

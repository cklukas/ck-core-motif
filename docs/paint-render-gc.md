# Buffered and cascade rendering GC failures

Buffered painting now abandons a paint transaction safely when it cannot obtain
its required GC, and retries acquisition on a later expose. Modern cascade
buttons use their ordinary LabelG expose path if their private face GC cannot
be created, then still restore the frame and cascade arrow. These failure
paths preserve widget state and avoid invalid X requests.

The focused source-contract regression is
`tests/automated/check-paint-render-gc.py`.

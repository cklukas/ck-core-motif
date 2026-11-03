# Shared appearance drawing GC failures

The modern button focus, button face, and cascade-indicator helpers allocate
short-lived X graphics contexts. Each helper now treats that allocation as
fallible. Button helpers release their temporary clipping region before
returning; cascade indicators simply skip that paint pass. No public resource,
widget state, or rendering contract changes, and a later expose retries the
draw normally.

The focused source-contract regression is
`tests/automated/check-appearance-draw-gc.py`.

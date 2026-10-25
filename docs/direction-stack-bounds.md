# Compound-text direction-stack bounds

Compound-text decoding grows a direction stack when nested direction controls
are encountered. `_PushDir` now checks both native multiplication and Xt’s
`Cardinal` allocation width before `XtRealloc`; an unrepresentable growth
request retains the existing conversion-failure result.

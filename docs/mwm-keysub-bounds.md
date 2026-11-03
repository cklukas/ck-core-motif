---
nav_exclude: true
---
# MWM key-substitution bounds

Localized MWM key substitutions now validate table growth against Xt's
allocation width and retain parsed token lengths in `size_t` until their
documented `int` representation is proven safe.

The replacement token is also limited so that its trailing NUL fits in Xt's
`Cardinal` allocation width. Invalid or unrepresentable replacements discard
the pending source token and leave the already completed substitutions intact.

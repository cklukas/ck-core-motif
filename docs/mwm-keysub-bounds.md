---
nav_exclude: true
---
# MWM key-substitution bounds

Localized MWM key substitutions now validate table growth against Xt's
allocation width and retain parsed token lengths in `size_t` until their
documented `int` representation is proven safe.

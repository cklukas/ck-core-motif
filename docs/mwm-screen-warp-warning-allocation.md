---
nav_exclude: true
---

# MWM screen-warp warning allocation widths

The `f.screen` command reports an unmanaged-screen target with a formatted
warning. Its `snprintf` length is now checked for negative and
`UINT_MAX`-unrepresentable results before conversion to an Xt `Cardinal`.
Normal warning text, screen selection, and the bell behavior are unchanged;
only formatting failure uses the existing plain fallback message.

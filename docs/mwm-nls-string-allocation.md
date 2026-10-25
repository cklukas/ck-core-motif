---
nav_exclude: true
---

# MWM catalog-string allocation

Localized MWM titles and warning-format strings are copied because `catgets`
may reuse its storage on the next lookup. `InitNlsStrings` now routes all six
copies through one helper that rejects null or unrepresentable lengths before
converting to Xt's `Cardinal` allocation width. Existing localized fallbacks
and warning behavior remain unchanged.

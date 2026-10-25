---
nav_exclude: true
---

# MWM catalog-string allocation

Localized MWM menu and title strings now share a checked copy helper. Catalog
values whose lengths cannot be represented by Xt's `Cardinal` allocation type
fail through the existing fallback paths instead of wrapping an allocation.

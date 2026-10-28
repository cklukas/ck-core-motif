---
nav_exclude: true
---

# AutoSuggestBox callback guards

AutoSuggestBox validates callback closures before reading private search/list
state. Foreign or stale closures are ignored safely across query, commit,
selection, and event callbacks while normal suggestions remain unchanged.

---
nav_exclude: true
---

# AutoSuggestBox callback guards

AutoSuggestBox callbacks can be reached while a popup or its client is being
destroyed, and applications can also invoke callbacks without an event. Query,
commit, selection, and key-event handlers now treat missing client or event
data as a no-op while retaining the existing behavior for live widgets.

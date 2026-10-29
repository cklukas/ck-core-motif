---
nav_exclude: true
---
# AutoSuggestBox child allocation

AutoSuggestBox creates a search field and a popup list for suggestions.
Popup creation, geometry, callbacks, keyboard navigation, dismissal, and
commit operations now tolerate either child being unavailable and clean up a
partially created popup safely.

The contract is protected by
`tests/automated/check-auto-suggest-child-failure-guards.py`.

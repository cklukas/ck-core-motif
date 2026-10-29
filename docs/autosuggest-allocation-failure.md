---
nav_exclude: true
---
# AutoSuggestBox suggestion-array allocation failure

`XmAutoSuggestBoxSetSuggestions` validates the suggestion-count multiplication
before opening or replacing the popup model. An oversized request is rejected
without erasing the current suggestions. Allocation failures after replacement
begins retain the existing behavior and leave the widget in a valid empty state
that can be repopulated by a later generation.

The focused source-contract check is
`tests/automated/check-autosuggest-input-preservation.py`.

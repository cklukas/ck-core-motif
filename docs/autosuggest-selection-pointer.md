# AutoSuggestBox selected-position result

When a search result is committed, AutoSuggestBox obtains the selected List
position and converts it to its zero-based item index.  A successful query can
still leave the returned array unavailable after allocation failure.  The
commit callback now requires both the single-position count and a non-null
array before reading `positions[0]`; valid selections are unchanged.

The regression contract is checked by
`tests/automated/check-autosuggest-selection-pointer.py`.

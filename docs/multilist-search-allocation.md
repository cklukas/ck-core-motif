# MultiList search allocation failure

`XmMultiList` now abandons a search safely when the text-field string cannot
be allocated.  It releases the optional wide-string result and leaves the
previous search term unchanged instead of passing NULL into comparison and
row-search code.

The focused source regression is
`tests/automated/check-multilist-search-allocation.py`.

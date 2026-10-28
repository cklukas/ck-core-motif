# DropDown match-position results

`SetListFromText` centers the first selected item by querying its List match
positions.  A positive match count does not guarantee that the returned array
is available after an allocation failure.  The positioning path now requires
both a positive count and a non-null array before reading `pos_list[0]`; when
the result is unavailable it retains the existing safe default position.

The regression contract is checked by
`tests/automated/check-dropdown-match-positions.py`.

# SearchBox clear callback lifetime

`XmSearchBoxClear` now snapshots both callback lists and the empty query before
dispatching the cleared notification. The subsequent query-changed notification
does not read widget state after user code runs, so a cleared callback may safely
destroy or replace the SearchBox without creating a stale-widget access path.

# MWM refresh tree cleanup

`RefreshByClearing` recursively queries child windows with `XQueryTree` and
must release the server-allocated child list after each successful query. The
cleanup now follows the returned pointer rather than the child count, so an
unusual successful response with an allocated empty list is still released.

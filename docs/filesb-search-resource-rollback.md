# FileSelectionBox search-resource rollback

`XmFileSelectionBox::SetValues` now checks copies of the directory, pattern,
and filter-mask strings before reading their lengths or scheduling a search.
For in-progress searches, replacement copies are installed before the old
values are released.  A failed copy keeps the previous search resource and
does not start a search with incomplete input.

The focused regression is
`tests/automated/check-filesb-search-resource-rollback.py`.

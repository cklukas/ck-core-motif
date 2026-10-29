# I18List selection and search input guards

`XmI18ListGetSelectedRows` now returns `NULL` for a NULL widget before row
storage is accessed. `XmI18ListDoSearch` rejects a NULL widget or search string
before creating a compound string or traversing rows. Valid selection and
multilingual search behavior is unchanged. `XmI18ListFindRow` also validates
its search string and output-column pointer.

The focused source contract is checked by
`tests/automated/check-i18list-search-inputs.py`.

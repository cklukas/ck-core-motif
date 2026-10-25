# XmString parse-mapping allocation failures

`XmParseMappingCreate` now checks its `XtNew` result before initialization.
When allocation fails it returns `NULL`, matching the existing setter's
failure-safe behavior and avoiding a null dereference during parser setup.

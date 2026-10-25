# BaseClass indirection-table bounds

`CreateIndirectionTable` now uses a `Cardinal` resource index and checks the
resource-count-to-byte conversion before allocating the pointer table. Invalid
or unrepresentable sizes return `NULL`; normal resource indirection is
unchanged.

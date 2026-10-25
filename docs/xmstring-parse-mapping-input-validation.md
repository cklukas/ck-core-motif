# XmString parse-mapping input validation

`XmParseMappingSetValues` now ignores invalid calls with a nonzero argument
count and no argument list, and skips entries whose argument name is null.
Valid mappings retain their existing updates; malformed entries no longer reach
`strcmp` through a null pointer.

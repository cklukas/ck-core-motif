# String-table resource conversion bounds

The string-to-string-table resource converter grows a pointer table while it
parses comma-separated values.  Both capacity paths now check the `int`
limit, release strings already parsed, and return conversion failure before
the capacity can wrap and cause an undersized reallocation.

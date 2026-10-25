# XmString parse-mapping inputs

`XmParseMappingSetValues` now rejects a null argument array when the requested
count is nonzero and skips entries with null resource names. Valid mappings and
zero-count calls retain their existing behavior, while malformed input leaves
the mapping unchanged for that entry.

# XmString pattern conversion

When a parse pattern and input use different text representations,
`match_pattern` converts the wide character through `wctomb`.  Conversion
failure is now treated as a non-match in both directions, so an unrepresentable
character cannot make the matcher inspect an uninitialized temporary buffer.

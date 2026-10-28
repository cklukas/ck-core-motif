# UIL compound-string parser constructors

The parser creates compound-string nodes for expressions and component
values.  Both constructor call sites now check for allocation failure before
writing expression or component metadata, allowing the semantic action to
return cleanly when memory is unavailable.

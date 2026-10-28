# UIL semantic coercion constructors

Argument and procedure validation coerces localized and primitive values into
compound or wide-string semantic nodes.  Every coercion constructor call now
checks its result before writing expression metadata; failed allocations cause
the enclosing validation pass to return cleanly instead of dereferencing a
null node.

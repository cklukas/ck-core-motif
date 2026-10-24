# Representation-type action parameters

Action parameter conversion now rejects a missing parameter or result pointer
before parsing.  Its whitespace and digit checks also pass unsigned byte
values to the C ctype functions, so malformed or non-ASCII input fails safely.

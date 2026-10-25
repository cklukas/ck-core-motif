# XmString helper lock boundaries

`XmStringTableToXmString` delegates to string-copy and concatenation APIs that
manage the Motif process lock themselves; it no longer wraps those calls in a
second lock. Parse-table cleanup likewise delegates mapping and string frees
without holding an outer lock. This prevents recursive-lock deadlocks while
preserving the existing ownership behavior.

The table parse and unparse helpers follow the same rule: their per-string
conversion APIs own the process lock, so the table loops no longer add an
outer lock.

Parse-mapping value access follows the same rule when copying a substitute
string: the mapping helper does not hold an outer process lock around
`XmStringCopy`.

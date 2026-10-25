# XmString helper lock boundaries

`XmStringTableToXmString` delegates to string-copy and concatenation APIs that
manage the Motif process lock themselves; it no longer wraps those calls in a
second lock. Parse-table cleanup likewise delegates mapping and string frees
without holding an outer lock. This prevents recursive-lock deadlocks while
preserving the existing ownership behavior.

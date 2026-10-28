# UIL string-table compound conversion

During semantic evaluation, narrow or localized string-table segments are
converted through a temporary compound-string node.  The evaluator now checks
that node before appending or copying it, returning cleanly if allocation
fails instead of dereferencing a null temporary value.

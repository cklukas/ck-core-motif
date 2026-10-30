# Xpm hash growth rollback

When an Xpm hash table reaches its load threshold, insertion now rolls back
the newly allocated atom if table growth fails. The failure still reports
`XpmNoMemory`, but the table no longer retains an uncounted entry or leaks the
atom allocation.

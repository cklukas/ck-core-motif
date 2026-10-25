# Withdrawal workspace guards

WSM client withdrawal can race workspace membership teardown.  The withdrawal
paths now verify that the client still has screen and workspace-list state,
and only pass a workspace to `TakeClientOutOfWorkspace` when the lookup
succeeds.  This keeps normal membership removal unchanged while avoiding a
null workspace dereference during partial teardown.

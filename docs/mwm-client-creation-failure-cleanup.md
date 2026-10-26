# MWM client-creation failure cleanup

`GetWmClientInfo` allocates a client record before creating icon-box or
confirmation-window state. If either creation or geometry discovery fails
before the client is registered, the function now releases that record before
returning `NULL`.

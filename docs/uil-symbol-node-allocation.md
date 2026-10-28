# UIL symbol-node allocation failures

Semantic actions create every symbol-table node through
`sem_allocate_node`.  The allocator now checks the `XtCalloc` result before
initializing the node header or linking the node into the allocated-node list.
An allocation failure is reported through Xt and returns `NULL`, avoiding the
allocator's own null dereference and preserving the existing error-handling
contract for callers.

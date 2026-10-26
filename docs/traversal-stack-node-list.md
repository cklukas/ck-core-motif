# Traversal node-list stack buffers

Traversal sorting collects graph nodes in temporary arrays through
`XmStackAlloc`, whose heap path narrows the byte count to `unsigned`.  The
nearest-node, tab-graph, and control-graph paths now reject an unrepresentable
`num_nodes * sizeof(XmTraversalNode)` size and return cleanly when allocation
fails, before writing the temporary list.

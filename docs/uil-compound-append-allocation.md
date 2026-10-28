# UIL compound-string append allocations

Compound-string operations can merge existing segments or copy them into a
new node.  The append and concatenate paths now validate their input nodes,
check copied-node allocation, and check the newly allocated concatenated
payload.  A failed payload allocation releases the node and returns the
existing semantic error value instead of dereferencing or copying through
`NULL`.

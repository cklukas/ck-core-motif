---
nav_exclude: true
---
# Virtual-binding converter allocation

The `XmRVirtualBinding` string converter allocates the output
`XmKeyBindingRec` array when the Xt conversion caller does not provide one.
The allocation is now checked before the array is populated.  On failure the
temporary keysyms, modifiers, and event-type buffers are released, the output
size is reset to zero, and conversion returns `False` instead of dereferencing
a null result.  Successful conversion and the caller-provided output-buffer
contract are unchanged.

# Container node allocation

`XmContainer` records each icon child in a private `CwidNode` linked list.
The constraint initializer now treats failure to allocate that node as a
normal initialization failure: it leaves the child unlinked and returns before
the node is dereferenced.  The existing container and child resources remain
owned by Xt, and destruction, layout, selection, and query paths tolerate the
missing private node.  If a new child refers to a parent that has no node, the
child is retained as a root item rather than being linked through a null
parent.

This preserves the public Container resource and child-widget contract while
avoiding a null dereference when an application creates a child during memory
pressure.

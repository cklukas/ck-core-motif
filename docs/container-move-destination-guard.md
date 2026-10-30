# Container drag destination validation

The internal drag-move callback now stops when the transfer has no destination
callback record. Transfer lookup can fail for a stale or malformed transfer;
continuing in that case would dereference missing state after moving an item.

Valid drag-and-drop transfers continue through the existing placement path.

# List selection rebuild allocation failures

The List widget now checks the allocations used while rebuilding selected item
and selected position arrays. If either allocation fails, the corresponding
selection count is reset to zero and the widget keeps an empty, consistent
selection state instead of writing through a null pointer. Successful rebuilds
retain their existing ordering and selection behavior.

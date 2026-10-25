# List selection callback allocation failures

Multiple and extended List selection callbacks receive temporary copies of
selected items and positions. The widget now checks both temporary allocations
before populating them. If either allocation fails, both arrays are released
and the callback receives the existing empty-array state rather than a partial
or invalid pointer.

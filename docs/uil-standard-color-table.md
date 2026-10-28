# UIL standard color-table singleton

The standard color table is initialized lazily and contains both a semantic
node and a two-element color array.  The constructor now checks both
allocations, releases a partial singleton, and returns the existing semantic
error entry when memory is unavailable.

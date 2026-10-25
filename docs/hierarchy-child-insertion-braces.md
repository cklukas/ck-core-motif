# Hierarchy child insertion control flow

The hierarchy widget's `AddChildToList` routine nests an insertion search,
its sibling loop, and the matching-child branch. One closing brace was
missing after the search, so later helper definitions were parsed as nested
functions and clean builds failed with invalid-storage-class errors.

The missing brace is restored. Child ordering, duplicate handling, and the
public hierarchy API are unchanged; this is a source-structure correction.

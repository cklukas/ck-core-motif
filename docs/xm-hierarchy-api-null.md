# XmHierarchy API null-safety

`XmHierarchyOpenAllAncestors` now returns for a null node, and
`XmHierarchyGetChildNodes` returns `NULL` before app-context handling. Valid
ancestor updates and child-list ownership remain unchanged.

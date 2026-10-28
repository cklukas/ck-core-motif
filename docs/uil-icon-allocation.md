# UIL icon allocation failure handling

`sar_make_icon` now checks both allocations used to construct an icon value:
the semantic value node and its `sym_icon_element` payload.  If either
allocation fails, a partially created node is released, the target is marked
as a semantic error, and the existing shared error entry is installed.  The
normal row and forward-reference setup is performed only for a fully
constructed icon.

This keeps malformed or resource-constrained UIL compilation on the existing
diagnostic path instead of dereferencing a null allocation result.

The source-contract regression is `tests/automated/check-uil-icon-allocation.py`.

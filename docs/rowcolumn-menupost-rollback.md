# RowColumn menu-post rollback

RowColumn now allocates a private copy of a changed `XmNmenuPost` string
before releasing the old value.  If the copy fails, the old menu-post string
and its parsed button/modifier mapping remain installed, and passive grabs are
left unchanged.  Clearing the resource still follows the existing update
path.

The focused regression is
`tests/automated/check-rowcolumn-menupost-rollback.py`.

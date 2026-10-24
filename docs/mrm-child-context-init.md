# Mrm child-tree context initialization

Widget-tree creation now checks the resource-context allocation used to load
children.  If that allocation fails, the tree operation returns an error before
attempting child reads through a null context.  Successful child loading and
all public Mrm interfaces remain unchanged.

The source-level regression is
`tests/automated/check-mrm-child-context-init.py`.

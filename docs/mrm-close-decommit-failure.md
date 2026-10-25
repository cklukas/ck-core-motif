# Mrm close retry after decommit failure

`UrmIdbCloseFile` now returns the decommit error while retaining the valid
`IDBFile` descriptor when a dirty buffer cannot be written. The associated
buffer remains attached and can be retried by the caller. Class and resource
table pointers are cleared immediately after their storage is released, so a
later close attempt cannot free those tables twice.

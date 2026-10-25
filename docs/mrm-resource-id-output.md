---
nav_exclude: true
---
# Mrm resource-id output validation

`UrmIdbGetResourceId` writes the next resource identifier through
`resource_id_return`. It now rejects a null output pointer before validating the
file and before the RID allocator runs, preventing a null write while retaining
valid identifier allocation behavior.

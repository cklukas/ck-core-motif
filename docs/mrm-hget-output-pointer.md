# Mrm hierarchy resource output validation

`UrmHGetIndexedResource` writes the file that supplied a resource through
`file_id_return`. It now rejects a null output pointer before traversing the
hierarchy, matching the lower-level file-open APIs and preserving valid lookup
behavior.

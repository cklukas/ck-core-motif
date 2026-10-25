# Mrm hierarchy name API input checks

The hierarchy name-registration and lookup helpers now validate the hierarchy
handle before dereferencing it. Registration also rejects null names in a
non-empty batch, and lookup rejects null names or output pointers. Valid
registrations and lookups retain their existing hash-table and global-fallback
behavior.

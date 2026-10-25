# Mrm open output initialization

`UrmIdbOpenFileRead` and `UrmIdbOpenBuffer` now clear the caller's output handle
immediately after validating the output pointer. Failed opens therefore cannot
leave a stale descriptor that a caller might mistake for the newly opened file;
successful opens still replace the output with the new descriptor as before.

# UIL WMD path setup failures

`init_wmd_path` now rejects a missing database filename, an unavailable home
directory expansion, formatting sizes that cannot be represented by Xt's
allocation interface, and WMD-path allocation failure.  `db_open_file` stops
before pathname resolution when that setup fails, and also returns after an
unresolved pathname or failed database-file open.  Valid absolute paths,
`WMDPATH`, and the historical default search-path expansion remain unchanged.

This prevents error handling for a missing or unallocatable WMD search path
from passing null pointers to the path resolver or file opener.

The account lookup used when neither `HOME` nor `USER` is available also keeps
the process ID in `uid_t`, avoiding a narrowing conversion before `_XGetpwuid`
selects the fallback home directory.

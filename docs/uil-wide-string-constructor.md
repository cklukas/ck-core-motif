# UIL wide-string constructor allocation

`sem_create_wchar_str` creates the semantic node used for wide-string values.
It now checks the central node allocator before initializing the node and
returns `NULL` on allocation failure, matching the compound-string constructor
contract.

# Mrm callback item binding failures

The callback item setters now check context validation and every callback
descriptor binding. This matters after appending routine names or callback
values, because appends can relocate the resource record. A failed rebind is
returned to the caller instead of dereferencing stale callback-item pointers.

The source-level regression check is
`tests/automated/check-mrm-callback-item-binding.py`.

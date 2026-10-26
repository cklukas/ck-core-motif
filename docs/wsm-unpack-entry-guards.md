# WSM unpack entry guards

The WSM request and reply unpackers are shared protocol boundaries.  They now
clear their destination records before decoding, reject a null destination,
and reject missing payloads except for the protocol message types whose
payload is explicitly empty (`WSM_CONFIG_FMT` requests and `WSM_SET_STATE`
replies).

Malformed callers therefore leave a deterministic, unallocated record for
`FreeRequest`/`FreeReply` instead of dereferencing null or stale storage.

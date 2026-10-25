# Mrm hierarchy open input validation

`Urm__OpenHierarchy` accepts an optional pointer to platform-specific open
parameters. A non-null outer pointer must contain a valid parameter object; the
old path dereferenced a null object before it could report an error. The input
check now rejects that malformed combination while keeping the existing null
outer-pointer behavior for callers that do not use platform-specific options.

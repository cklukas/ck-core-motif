# Virtual-key binding table growth

`FillBindingsFromDB` appends resource-derived virtual-key bindings to a shared
table.  Growth now checks count addition and byte multiplication against both
`size_t` and Xt's `Cardinal` allocation range, uses a temporary realloc pointer,
and releases converted input before returning on failure.  Existing bindings
remain published when a new allocation cannot be made.

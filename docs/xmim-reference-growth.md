# Input-method reference-table growth

XIM widget references now grow their widget and callback arrays as one
transaction. If either allocation fails, both existing arrays remain unchanged
and `add_ref` reports failure without inserting a partial reference. Growth
checks `Cardinal` and `size_t` arithmetic before allocating, and guarantees
progress even if a damaged or legacy record has a one-entry capacity.

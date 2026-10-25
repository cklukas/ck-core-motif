# Input-method reference-table growth

XIM widget references now grow their widget and callback arrays as one
transaction. If either allocation fails, both existing arrays remain unchanged
and `add_ref` reports failure without inserting a partial reference.

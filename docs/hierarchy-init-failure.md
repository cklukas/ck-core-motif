# Hierarchy initialization failure

Hierarchy now initializes its ownership fields before allocating the root
constraint node. If that allocation fails, the widget remains in a valid
empty state, and destruction skips the absent child array instead of
dereferencing a null root node. Normal hierarchy construction is unchanged.

# MWM outline-window allocation

The WSM outline renderer uses four temporary windows for the north, south,
east, and west edges. Creation is now transactional: if any edge window
cannot be allocated, already-created siblings are destroyed and all IDs are
reset. `WindowOutline` draws only when all four resources are valid, avoiding
partial outlines and invalid X requests.

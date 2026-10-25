# Cardinal-list resource conversion allocation

The Cardinal-list resource converter now checks its initial array and every
growth reallocation. A failed allocation follows the existing conversion
warning path and releases the array already built, while successful numeric
conversion remains unchanged.

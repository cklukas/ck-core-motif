# Internal list-stack allocation failures

The internal cleanup stack now checks null stack handles and failed growth
reallocations. A failed growth leaves the existing stack and its top index
unchanged, so cleanup code does not write through a null buffer.

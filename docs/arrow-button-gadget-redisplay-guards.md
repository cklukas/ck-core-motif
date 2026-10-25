# ArrowButtonGadget redisplay guards

`XmArrowButtonGadget` can be exposed while its manager resources are being
replaced or while highlight and shadow margins leave no drawable area.  Its
background, shadow, and arrow helpers previously accepted missing GCs and
zero-sized geometry.

The redisplay and shared arrow helper now skip only invalid intermediate
requests.  Input-driven arm and activate redraws use the same safe helper,
while valid gadget rendering and callback behavior remain unchanged.

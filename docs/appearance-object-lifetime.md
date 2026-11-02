---
nav_exclude: true
---
# Appearance and scale object lifetime

Only Widgets and RectObj gadgets participate in appearance/scale registration.
Private extension/cache Objects can be freed without the ordinary Xt visual
object lifecycle and must not enter these registries.

The registries observe Xt's display-level `destroyHook`. This phase-one signal
precedes hierarchy memory reclamation and is independent of the application's
mutable `destroyCallback` resources. Removing or replacing those resources cannot
leave a stale gadget pointer for a later appearance or scale transaction.

Destruction retires and immediately unlinks the record by object identity.
Existing transaction snapshots keep their references and defer the record's
freeing until the last reference is released. Display teardown releases remaining
registry records and hook subscriptions without querying or removing callbacks
through possibly reclaimed object addresses. Public widget record layouts are
unchanged.

The native visual-object lifetime fixture rejects nonvisual registration,
retains real widget/gadget registration, removes application-visible destroy
callbacks, exercises gadget replacement and Container updates, then destroys the
hierarchy and display. The Periodic fixture checks the original failing large
UIL hierarchy teardown across eleven appearance/density/startup configurations.

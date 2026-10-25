# Skeleton loading presentation

`XmSkeleton` is a bounded placeholder for content that is loading.  It owns
no data, timer queue or application state.  Animation is opt-in presentation
only, is disabled by `XmNskeletonReducedMotion`, and has a per-instance
interval; `XmSkeletonStop` always provides a static fallback.  Applications
choose the placeholder geometry and replace it with their real view.

The label is a copied `XmString` used for accessibility; it is not drawn into
the placeholder.  The widget does not introduce a generalized animation
framework or require animation for a usable loading state.  The installed
fixture covers start/stop, reduced-motion behavior, accessibility naming,
timer teardown and destruction.

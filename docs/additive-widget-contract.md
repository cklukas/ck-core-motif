# Additive widget contract

New controls are ordinary opt-in Motif widget classes. They use the existing
Xt object, resource, geometry, traversal, callback, UIL/WML and display
ownership rules; they do not introduce a parallel runtime or alter the
meaning of an existing resource or callback record.

## Public-class rules

- Each class gets a distinct public header, class symbol, constructor naming
  convention and resource/callback namespace.
- Resources describe persistent widget state. Per-view or transient state is
  not encoded in global variables or application-wide singletons.
- A widget owns its Xt children and cancels timers, work procedures and
  registered callbacks before destruction. Borrowed widgets are validated at
  the point of use and never retained past their owner’s lifetime.
- Geometry uses normal Xt negotiation. Explicit application dimensions win
  over fallback resources; scaled metrics are opt-in and must not reinterpret
  legacy pixel resources silently.
- Activation, value changes, cancellation and sensitivity have documented
  keyboard and pointer behavior. Typed callback records are added only when a
  generic callback cannot express the event without ambiguity.
- Callback-driven destruction is a required test case. No callback may require
  its widget or parent to remain alive after the callback returns.

## Accessibility and UI-thread rules

Every class documents its accessible role, name/value/state exposure,
actions, focus behavior and change notifications. Native accessibility
providers consume those semantics; a provider’s absence does not change the
widget’s state contract. All widget, display, resource and callback operations
remain on the owning Xt thread. A new widget must not create worker threads or
an animation loop; bounded Xt timers are permitted for visible indeterminate
feedback and are canceled before unmap or destruction.

## Delivery harness

`tests/automated/run-additive-widget-contract.py` compiles
`check-additive-widget-contract.c` against one absolute installed prefix,
checks that `libXm` and `libXt` resolve from that prefix, and runs the fixture
under an isolated Xvfb. The fixture deliberately uses existing public
PushButton/List classes as contract probes until WP-16 supplies the first new
class. It checks explicit-over-fallback resource precedence, keyboard focus
traversal, the public list accessibility-change callback, activation callback
teardown, and clean application-context shutdown.

The fixture is a contract gate, not a claim that every existing widget or an
external assistive client is complete. A new class must add its own focused
normal, invalid-input, resize, keyboard, accessibility and destruction cases.

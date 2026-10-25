# Reusable native components

Reusable widgets belong in the toolkit when their interaction, geometry,
keyboard behavior and semantic contract are useful across independent Xt
applications. An application-specific document rule or workflow belongs with
its model or application owner.

## Component requirements

A native component needs a public resource/callback contract, explicit child
ownership, geometry negotiation, keyboard and focus behavior, lifecycle cleanup
and an accessibility projection. Its tests should exercise actual widget
behavior, multiple instances, resizing and destruction; a screenshot or an
unused API declaration is insufficient.

Unicode paragraph/capture facilities and accessibility providers are examples
of generic reusable services already implemented in the development source.
They separate immutable values and lifetime rules from application behavior.

## Current and future widgets

| Need | Toolkit facility | Reuse boundary |
| --- | --- | --- |
| Collapsible inspector/settings sections | `XmDisclosure` | Stable child ownership, focus transfer and expansion state; application owns the section contents. |
| Labelled form sections | `XmFormSection` | Toolkit owns header/footer presentation and the content host; applications own content children and form/data semantics. |
| Empty content views | `XmEmptyState` | Toolkit owns optional icon/message/action presentation; applications own the empty predicate and action behavior. |
| Transient notifications | `XmToast` | Toolkit owns one bounded per-instance presentation and timer; applications own placement, queue policy and action behavior. |
| Command discovery | `XmCommandPalette` | Toolkit owns copied labels, filtering and focus/lifecycle presentation; applications own command IDs, model policy and dispatch. |
| Numeric navigation | Native ScrollBar with accessible Value/actions | Application maps its model range to the native control; toolkit owns native interaction and semantics. |
| Unicode fields and editors | Text/TextField plus bounded text contexts | Toolkit owns input, shaping and widget projection; application owns document operations and undo. |
| Custom canvases | Accessibility provider API | Application supplies semantic objects; transport and native lifetime remain shared toolkit services. |

The development [disclosure section API](disclosure.md) provides a stable
content host, explicit expansion state and transition callbacks. Its public
contract and installed native ARM64 fixture results are documented together.
Application usability and other-architecture acceptance remain separate.

Other recurring patterns can justify future shared components: labelled field
rows, searchable selectors, notification/status areas and split-pane navigation.
These are design candidates, not available APIs or promised release features.
Each should be implemented only with a concrete reusable contract and
meaningful native tests. Prefer composition over copying application-specific
panels into the toolkit.

## Window-owned modal dialogs

Use `XmDIALOG_PRIMARY_APPLICATION_MODAL` for a dialog that blocks its ancestor
windows while independent top-level windows remain usable. The scope is enforced
by the client toolkit and does not require a particular window manager. Nested
primary dialogs block their parent dialogs too. Parallel primary dialogs each
retain their own ancestor restrictions.

Use `XmDIALOG_FULL_APPLICATION_MODAL` when unrelated windows in the same
application must also be blocked. New top-level windows and unrelated modal
popups remain blocked until that enclosing modal scope is released. A primary
dialog nested inside a full modal does not reopen independent branches.

An application still owns dialog intent, target identity and workflow decisions.
Destroying or popping down a dialog must release its input restriction; it must
not approve or redirect an application operation. See the [native fixture
instructions](build.md) and [current validation scope](status.md).

## Adding a component

Start from a repeated interaction that existing widgets cannot express cleanly.
Check the inherited widget set before adding a new class. Keep reusable geometry,
input, presentation and accessibility behavior in the fork; keep file formats,
document rules and business operations in consumers.

Deliver the public header/resources/callbacks, implementation, installed-library
fixture, a minimal construction example and a documentation page together. Test
two independent instances, keyboard-only operation, resizing, focus transfer,
callback-triggered destruction and accessible state/action delivery. Qualify
the exact installed header/library cohort before adoption. Record changes in
[fork history](history.md) and distinguish source availability from verified
platforms in [status](status.md).

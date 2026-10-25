# AutoSuggestBox

`XmAutoSuggestBox` is an opt-in composite containing an ordinary
`XmSearchBox` and an anchored public `XmList` popup.  The application owns the
provider, filtering, I/O and UI-thread scheduling.  The toolkit never calls a
provider from drawing or popup layout code.

Every query change increments a monotonically increasing generation and sends
that generation with `XmNautoSuggestQueryChangedCallback`.  The application
must pass the same generation to `XmAutoSuggestBoxSetSuggestions`; older or
future generations are rejected, so a delayed result cannot replace a newer
query.  The widget copies suggestion IDs and labels and retains `user_data`
without taking ownership.

Down-arrow moves focus from the search field to the popup list, Escape
dismisses it, and the list's normal keyboard/pointer activation commits the
selected item through `XmNautoSuggestCommittedCallback`.  Dismissal is
reported separately and focus returns to the search field.  An empty accepted
result is shown as the configurable no-results label and is never actionable.
The popup is per-widget and is destroyed with its owner; no global suggestion
state or blocking work is introduced.

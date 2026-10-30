# SearchBox

`XmSearchBox` is an additive subclass of `XmTextField`.  It preserves the
ordinary text-field editing, selection, validation and IME paths while adding
three application callbacks: `XmNsearchQueryChangedCallback` for edits,
`XmNsearchCommittedCallback` for an explicit search submission, and
`XmNsearchClearedCallback` for the clear affordance.  The callback record
contains a borrowed query snapshot valid only for the callback duration.

`XmNsearchPlaceholder` is presentation-only: it is never inserted into the
text-field value or submitted.  Escape, the optional trailing clear button,
and `XmSearchBoxClear` clear the real value.  `XmSearchBoxCommit` only emits a
callback; the toolkit performs no blocking search or network work.  All
callbacks run on the owning Xt thread and applications own asynchronous work
and lifetime.

Accessibility exposes a text role with the current query length and a `commit`
action. When the clear button is enabled, the field is editable, and the query
is non-empty, it also exposes a `clear` action; read-only and empty fields do
not advertise or accept that action. The installed fixture covers placeholder
separation, query/clear/commit ordering, direct API operation, accessibility
action names, read-only behavior, sensitivity and teardown.

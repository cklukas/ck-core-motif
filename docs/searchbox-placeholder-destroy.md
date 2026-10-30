# SearchBox placeholder destruction

The SearchBox owns the copied `XmString` used for its placeholder and now
releases that copy in `Destroy`. This closes the remaining lifecycle leak while
preserving transactional replacement behavior when `XtSetValues` changes the
placeholder.

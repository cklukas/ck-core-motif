# Container detail-header allocation

Container copies icon detail headers into an internal table with a leading
label entry. The count is now checked before adding that entry and multiplying
by the `XmString` pointer size; an unrepresentable table is rejected without a
wrapped allocation.

The same checked multiplication is used when copying the `XmNdetailOrder`
array during initialization and `SetValues`.

# UTM send widget guard

`UTMSendMessage` now rejects a null widget before allocating or queueing a UTM
package.  This protects all WSM callers from passing a missing requestor to
`XmeNamedSink`.  The parameter payload remains caller-owned on this early
return, matching the existing ownership contract for failed sends.

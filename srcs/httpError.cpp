#include "httpError.hpp"
#include <iostream>

httpError::httpError(): msg(""), status(0) {}
httpError::httpError(status_code_t s, const string& m): msg(m), status(s) {}
httpError::~httpError() throw() {};
const char*	httpError::what() const throw() { return msg.c_str(); }

Created::Created(): httpError(201, "Created"), location() {}
Created::~Created() throw() {};

noContent::noContent(): httpError(204, "No Content"), location() {}
noContent::noContent(const string& loc): httpError(204, "No Content"), location(loc) {}
noContent::~noContent() throw() {};

redirectError::redirectError(): httpError(300, "Error"), location() {}
redirectError::redirectError(status_code_t s, const string& m, const string& loc): httpError(s, m), location(loc) {}
redirectError::~redirectError() throw() {};

movedPermanently::movedPermanently(): redirectError(301, "Moved Permanently", "") {}
movedPermanently::movedPermanently(const string& loc): redirectError(301, "Moved Permanently", loc) {}
movedPermanently::~movedPermanently() throw() {};

found::found(): redirectError(302, "Found", "") {}
found::found(const string& loc): redirectError(302, "Found", loc) {}
found::~found() throw() {};

seeOther::seeOther(): redirectError(303, "See Other", "") {}
seeOther::seeOther(const string& loc): redirectError(303, "See Other", loc) {}
seeOther::~seeOther() throw() {};

temporaryRedirect::temporaryRedirect(): redirectError(307, "Temporary Redirect", "") {}
temporaryRedirect::temporaryRedirect(const string& loc): redirectError(307, "Temporary Redirect", loc) {}
temporaryRedirect::~temporaryRedirect() throw() {};

permanentRedirect::permanentRedirect(): redirectError(308, "Permanent Redirect", "") {}
permanentRedirect::permanentRedirect(const string& loc): redirectError(308, "Permanent Redirect", loc) {}
permanentRedirect::~permanentRedirect() throw() {};

badRequest::badRequest(): httpError(400, "Bad Request") {}
badRequest::~badRequest() throw() {};

forbidden::forbidden(): httpError(403, "Forbidden") {}
forbidden::~forbidden() throw() {};

notFound::notFound(): httpError(404, "Not Found") {}
notFound::~notFound() throw() {};

methodNotAllowed::methodNotAllowed(): httpError(405, "Method Not Allowed") {}
methodNotAllowed::~methodNotAllowed() throw() {};

Conflict::Conflict(): httpError(409, "Conflict") {}
Conflict::~Conflict() throw() {};

lengthRequired::lengthRequired(): httpError(411, "Length Required") {}
lengthRequired::~lengthRequired() throw() {};

payloadTooLarge::payloadTooLarge(): httpError(413, "Payload Too Large") {}
payloadTooLarge::~payloadTooLarge() throw() {};

URITooLong::URITooLong(): httpError(414, "URI Too Long") {}
URITooLong::~URITooLong() throw() {};

internalServerError::internalServerError(): httpError(500, "Internal Server Error") {}
internalServerError::~internalServerError() throw() {};

notImplemented::notImplemented(): httpError(501, "Not Implemented") {}
notImplemented::~notImplemented() throw() {};

HTTPVersionNotSupported::HTTPVersionNotSupported(): httpError(505, "HTTP Version Not Supported") {}
HTTPVersionNotSupported::~HTTPVersionNotSupported() throw() {};

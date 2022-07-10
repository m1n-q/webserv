#ifndef HTTPERROR_HPP
# define HTTPERROR_HPP

#include <exception>
#include <string>
using namespace std;

typedef unsigned short status_code_t;

class httpError: public exception
{
	private:	string msg;
	public:		status_code_t status;
				explicit httpError();
				explicit httpError(status_code_t s, const string& m);
				virtual ~httpError() throw();
				virtual const char * what() const throw();
};

class badRequest: public httpError
{
	public:		explicit badRequest();
				explicit badRequest(status_code_t s, const string& m);
				virtual ~badRequest() throw();
};

class methodNotAllowed: public httpError
{
	public:		explicit methodNotAllowed();
				explicit methodNotAllowed(status_code_t s, const string& m);
				virtual ~methodNotAllowed() throw();
};

class notFound: public httpError
{
	public:		explicit notFound();
				explicit notFound(status_code_t s, const string& m);
				virtual ~notFound() throw();
};

class forbidden: public httpError
{
	public:		explicit forbidden();
				explicit forbidden(status_code_t s, const string& m);
				virtual ~forbidden() throw();
};

class internalServerError: public httpError
{
	public:		explicit internalServerError();
				explicit internalServerError(status_code_t s, const string& m);
				virtual ~internalServerError() throw();
};

class payloadTooLarge: public httpError
{
	public:		explicit payloadTooLarge();
				explicit payloadTooLarge(status_code_t s, const string& m);
				virtual ~payloadTooLarge() throw();
};

class lengthRequired: public httpError
{
	public:		explicit lengthRequired();
				explicit lengthRequired(status_code_t s, const string& m);
				virtual ~lengthRequired() throw();
};

class URITooLong: public httpError
{
	public:		explicit URITooLong();
				explicit URITooLong(status_code_t s, const string& m);
				virtual ~URITooLong() throw();
};


class Created: public httpError
{
	public:		string	location;
				explicit Created();
				explicit Created(status_code_t s, const string& m);
				virtual ~Created() throw();
};

class noContent: public httpError
{
	public:		string	location;
				explicit noContent();
				explicit noContent(const string& loc);
				virtual ~noContent() throw();
};

class Conflict: public httpError
{
	public:		string	location;
				explicit Conflict();
				explicit Conflict(status_code_t s, const string& m);
				virtual ~Conflict() throw();
};

class redirectError: public httpError
{
	public:		string	location;
				explicit redirectError();
				explicit redirectError(status_code_t s, const string& m, const string& loc);
				virtual ~redirectError() throw();
};

class movedPermanently: public redirectError
{
	public:		explicit movedPermanently();
				explicit movedPermanently(const string& loc);
				virtual ~movedPermanently() throw();
};

class found: public redirectError
{
	public:		explicit found();
				explicit found(const string& loc);
				virtual ~found() throw();
};

class seeOther: public redirectError
{
	public:		explicit seeOther();
				explicit seeOther(const string& loc);
				virtual ~seeOther() throw();
};
class temporaryRedirect: public redirectError
{
	public:		explicit temporaryRedirect();
				explicit temporaryRedirect(const string& loc);
				virtual ~temporaryRedirect() throw();
};
class permanentRedirect: public redirectError
{
	public:		explicit permanentRedirect();
				explicit permanentRedirect(const string& loc);
				virtual ~permanentRedirect() throw();
};

class notImplemented: public httpError
{
	public:		explicit notImplemented();
				explicit notImplemented(status_code_t s, const string& m);
				virtual ~notImplemented() throw();
};

class HTTPVersionNotSupported: public httpError
{
	public:		explicit HTTPVersionNotSupported();
				explicit HTTPVersionNotSupported(status_code_t s, const string& m);
				virtual ~HTTPVersionNotSupported() throw();
};

#endif

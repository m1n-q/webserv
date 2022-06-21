# include "core.hpp"
# include "checkFile.hpp"
# include "ConfigLoader.hpp"
# include "ConfigChecker.hpp"
#include "httpError.hpp"
#include <string>
# include <sys/stat.h>

void			core(PollSet& pollset, ServerSocket *serv, ConnSocket *connected)
{
	string			reqTarget = connected->ReqH.getRequsetTarget();
	string			ext	= getExt(reqTarget);
	string			method = connected->ReqH.getMethod();
	status_code_t	status = 42;
	pair <string, string> p = CHECK->routeRequestTarget(connected->conf, reqTarget);

	if (method == "PUT")
		status = createPutFile(connected, p);
	else if (method == "DELETE")
		status = deleteFile(p);
	else	
		status = writeResponseBody(connected, reqTarget, p);
	connected->ResH.setStatusCode(status);
	if (CONF->MIME.find(ext) != CONF->MIME.end())
		connected->ResH["Content-Type"]	= CONF->MIME[ext];
	else
		connected->ResH["Content-Type"] = connected->conf->default_type;
	if (!connected->ResB.getContent().empty())
		connected->ResH["Content-Length"]	= toString(connected->ResB.getContent().length());
	if (status == 201)
		connected->ResH.append("Location", p.first + p.second);
	if (status == 200 && getExt(reqTarget) == "py")
	{
		connected->ResB.clear();
		connected->ResH.removeKey("content-length");
		CGIRoutines(pollset, serv, connected, NULL);
		return ;
	}
}

void	core_wrapper(PollSet& pollset, ServerSocket *serv, ConnSocket *connected, Pipe* CGIpipe)
{
	if (!CGIpipe)
		core(pollset, serv, connected);
	else
	{
		CGIRoutines(pollset, serv, connected, CGIpipe);
		if (connected->pending)
			return;
	}
	if (!connected->ResH.getHeaderField().empty())
	{
		writeResponseHeader(connected);
		connected->ResH.makeStatusLine();
		connected->ResH.integrate();
	}
}

status_code_t	writeResponseBody(ConnSocket* connected, const string& reqTarget, pair<string, string> p)
{
	struct stat 	s;
	status_code_t	status;
	string			indexfile;
	string			prefix;
	string			uri;

	prefix = p.first;
	uri = p.second;

	string			filename = prefix + uri;

	try						{ s = _checkFile(filename); }
	catch (httpError& e)	{ throw; }	// for 404

	if (S_ISDIR(s.st_mode))
	{
	/**========================================================================
	 * @  if FOUND final (deepest) index file
	 * @  	if final index file was directory
	 * @		auto index on ? directory listing() : forbidden();
	 * @	else
	 * @		if final index file forbidden? forbidden();
	 * @		else print index file;
	 * '  else NOT FOUND
	 * '	auto index on ? directory listing() : forbidden();
	 *========================================================================**/
		if (reqTarget.back() != '/')
			throw movedPermanently("http://" + connected->ReqH["Host"] + reqTarget + '/');

		try							{ indexfile = findIndexFile(connected->conf, filename); }
		catch (httpError& e)		{ throw; }

		if (indexfile.back() == '/')
		{
			if (connected->conf->auto_index)
			{
				connected->ResB.setContent(directoryListing(indexfile, prefix));	/* alias case: need to append Loc URI || or req Target ? */
				return 200;
			}
			else
				throw forbidden();
		}
		filename = indexfile;
	}
	status = connected->ResB.readFile(filename);

	return status;
}
// path에 file이 없으면 404 Not Found
// 성공 하면 204 No Content(폴더여도 뒤에 /만 있으면 지워버림)
// chmod 000 권한이 없는대도 그냥 지워버린다?
// 폴더인데 뒤에 /가 없으면 409 Conflict
status_code_t	deleteFile(pair<string, string> p)
{
	int		ret;
	struct 	stat s;
	string	filename = p.first + p.second;

	try						{ s = _checkFile(filename); }
	catch (httpError& e)	{ throw; }	// for 403
	
	if (S_ISDIR(s.st_mode) && filename.back() != '/')
		return 409; // throw?
		 
	ret = remove(filename.c_str());
	if (ret == -1)
		return 404; // throw?
	return 204; // throw?
}
status_code_t	createPutFile(ConnSocket* connected, pair<string, string> p)
{
	int				create = -1;
	int				fd;
	int				byte;
	string			filename = p.first + p.second;

	create = access(filename.c_str(),F_OK);
	fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
	if (fd == -1)
		return 409; // throw로?
	byte = write(fd, connected->ReqB.getContent().c_str(), connected->ReqB.getContent().length());
	if (byte == static_cast<int>(connected->ReqB.getContent().length()) && create == 0)
		return 204; // throw로?
	else if (byte == static_cast<int>(connected->ReqB.getContent().length()))
		return 201; // throw로?
	// 아래의 경우 poll로 다시 돌아가야 함
	return 100; // throw로?
}

void	writeResponseHeader(ConnSocket* connected)
{
	connected->ResH.setHTTPversion("HTTP/1.1");
	if (connected->ResH.exist("status") == true)
	{
		pair<status_code_t, string>	sr = checkStatusField(connected->ResH["Status"]);
		connected->ResH.setStatusCode(sr.first);
		connected->ResH.setReasonPhrase(sr.second);
		connected->ResH.removeKey("Status");
	}
	else
	{
		switch (connected->ResH.getStatusCode())
		{
		case 200:	connected->ResH.setReasonPhrase("OK");			break;
		case 201:	connected->ResH.setReasonPhrase("Created");	break;
		case 204:	connected->ResH.setReasonPhrase("No Content");	break;
		case 404:	connected->ResH.setReasonPhrase("Not Found");	break;
		/* and so on ... */
		}
	}
	connected->ResH.setDefaultHeaders();
}

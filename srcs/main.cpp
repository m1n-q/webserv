#include <algorithm>
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <poll.h>
#include <string>
#include <sys/poll.h>

#include "CGI.hpp"
#include "Config.hpp"
#include "ConfigLoader.hpp"
#include "HttpConfig.hpp"
#include "ConfigUtils.hpp"
#include "ConnSocket.hpp"
#include "Pipe.hpp"
#include "Poll.hpp"
#include "ReqBody.hpp"
#include "ResBody.hpp"
#include "ReqHeader.hpp"
#include "ResHeader.hpp"
#include "ServerSocket.hpp"
#include "core.hpp"
#include "httpError.hpp"
#include "utils.hpp"


map<string, string> MIME;

int main(int argc, char** argv)
 {
	if (argvError(argc))
		return (errMsg());
	signal(SIGPIPE, SIG_IGN);
	// HttpConfig http;
	try
	{
		HttpConfig::getInstance()->setConfig(ReadConfig(argv));
		HttpConfig::getInstance()->defaultSet();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return -1;
	}

	CONF->setAddrs(HttpConfig::getInstance()->serverMap);
	CONF->loadMIME();


	//IMPL: create all serverSocket from m.key(ip:port), each socket has vector<ServerConfig*>
	ServerSocket*		serv;
	ConnSocket*			connected;
	Pipe*				CGIpipe;
	IStream*			stream;

	PollSet				pollset;
	PollSet::iterator	it;

	string				content;
	map<int, undone>	writeUndoneBuf;
	createServerSockets(CONF->getAddrs(), pollset);
	pollset.createMonitor();

	while (1)
	{
//'----------------------catch and parse request header----------------------'//
		try												{ whoDied();
														  it = pollset.examine(); }
		catch	(exception& e)							{ continue; }
		if		(CONVERT(*it.second, ServerSocket))		{ continue; }	// servSocket

		connected	= CONVERT(*it.second, ConnSocket);
		CGIpipe		= CONVERT(*it.second, Pipe);
		if (CGIpipe)
		{
			connected =  CGIpipe->linkConn;
			if (CGIpipe == connected->linkWritePipe)	// write to child process
			{
				stream = CGIpipe;
				content = connected->ReqB.getContent();
				goto _send;
			}
			goto _core;	// read from child process
		}

		if (it.first->revents & POLLOUT)	{ it.first->events &= ~POLLOUT;
									  		  goto _send; }

	try										{ connected->recvRequest(); }
	catch	(exception& e)					{
												if (CONVERT(&e, ConnSocket::connClosed) ||		//NOTE: what if client does not close after we send FIN? (cause we do graceful-close)
													CONVERT(&e, ConnSocket::somethingWrong))
												{
													connected->close();
													pollset.drop(it);
													continue;
												}

												if (CONVERT(&e, IStream::readMore))
												{
													continue;
												}

												httpError* err = CONVERT(&e, httpError);
												if (err)
												{
													cout << err << endl;
													connected->returnError(err->status, err->what());
													goto _set_stream_to_socket;
												}
											}

//'-------------------------------- catch end--------------------------------'//
_core:
		serv = connected->linkServerSock;
		try							{ core_wrapper(pollset, serv, connected, CGIpipe); }	//@ make response header, body//
		catch (httpError& e)		{
									  redirectError* r = CONVERT(&e, redirectError);
									  if (r) connected->ResH["Location"] = r->location;
									  connected->returnError(e.status, e.what());
								 	  goto _set_stream_to_socket;	}
		if (connected->pending)
			continue;
		if (!CGIpipe && connected->linkReadPipe)	/* pipe is just created, do not send 0 byte */
			continue;

_set_stream_to_socket:
		stream	= connected;
		content = connected->ResH.getContent() +
				  connected->ResB.getContent();


//.------------------------send response header, body------------------------.//
_send:
		try									{ stream->send(content, writeUndoneBuf); }
		catch (exception& e)				{
											  if (CONVERT(&e, IStream::sendMore))
												it.first->events |= POLLOUT; // not all data sended
											  else
												pollset.drop(it);
											}

		connected->ReqH.clear(), connected->ResH.clear(), connected->ResB.clear();
//.---------------------------------send end---------------------------------.//
	}
}







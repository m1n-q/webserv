#include <cstdio>
#include <iostream>
#include <signal.h>
#include <poll.h>

#include "CGI.hpp"
#include "Config.hpp"
#include "ConnSocket.hpp"
#include "Pipe.hpp"
#include "Poll.hpp"
#include "ReqBody.hpp"
#include "ResBody.hpp"
#include "ReqHeader.hpp"
#include "ResHeader.hpp"
#include "ServerSocket.hpp"
#include "core.hpp"



int main()
 {
	signal(SIGPIPE, SIG_IGN);

	ServerSocket		serv("", 8888);	// put your IP, "" means ANY
	ConnSocket*			connected;
	Pipe*				CGIpipe;

	PollSet				pollset;
	PollSet::iterator	it;

	map<int, undone>	undoneBuf;
	root += "/static_file";

	try						{ serv.bind(); }
	catch (exception& e)	{ cerr << e.what() << endl; exit(errno); }
	try						{ serv.listen(10 /*backlog*/); }
	catch (exception& e)	{ cerr << e.what() << endl; exit(errno); }

	pollset.enroll(&serv);
	cout << "hello" << endl;
	while (1)
	{
//'----------------------catch and parse request header----------------------'//
		try									{ it = pollset.examine(); }
		catch	(exception& e)				{ continue; }
		if		((*it.second) == &serv)		{ continue; }	// servSocket

		connected	= dynamic_cast<ConnSocket*>(*it.second);
		CGIpipe		= dynamic_cast<Pipe*>(*it.second);
		if (CGIpipe)
		{
			connected =  CGIpipe->linkConn;
			goto corew;
		}

		if (it.first->revents & POLLOUT)	{ it.first->events &= ~POLLOUT;
									  		  goto resend; }

		try									{ connected->recvRequest();}
		catch	(exception& e)				{ continue; }
		if		(connected->ReqH.empty())	{ connected->close();
									  		  pollset.drop(it);
									  		  continue; }	// client exit

//'-------------------------------- catch end--------------------------------'//
corew:
		core_wrapper(pollset, &serv, connected, CGIpipe);	//@ make response header, body//
		if (connected->pending)
			continue;
		else if (CGIpipe) pollset.drop(it);	// pended CGI output ends.

//.------------------------send response header, body------------------------.//
resend:
		try									{ connected->send(
																connected->ResH.getContent() +
																connected->ResB.getContent(),
																undoneBuf
															);
											}
		catch (exception& e)				{ it.first->events |= POLLOUT; }	// not all data sended

		connected->ReqH.clear(), connected->ResH.clear(), connected->ResB.clear();
//.---------------------------------send end---------------------------------.//
	}
}







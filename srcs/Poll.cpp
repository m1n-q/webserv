#include "Poll.hpp"
#include "FileStream.hpp"
#include "Pipe.hpp"
#include "Stream.hpp"
#include "Timer.hpp"
#include <ios>

PollSet*	PollSet::pollset;

/**========================================================================
* @                           Constructors
*========================================================================**/

PollSet::PollSet(): pollVec(), pollMap(), timer(NULL) { }
PollSet::~PollSet() {}


/**========================================================================
* #                          member functions
*========================================================================**/


void		PollSet::enroll( Stream* stream, short events )
{
	Poll		p;
	p.fd		= stream->getFD();
	p.events	= events;
	p.revents	= 0;

	stream->setTimeOut(20);
	stream->updateLastActive();

	pollMap[p.fd] = make_pair(p, stream);

	ServerSocket*	serv		= CONVERT(stream, ServerSocket);
	ConnSocket*		connected	= CONVERT(stream, ConnSocket);
	FileStream*		filestream	= CONVERT(stream, FileStream);
	Pipe*			P			= CONVERT(stream, Pipe);

	TAG(PollSet, enroll); cout << GRAY("Enroll ") << stream->getFD()
	<< (serv		? CYAN( " (ServerSocket) ") "[" _UL + serv->getIP() + ":" + toString(serv->getPort()) + _NC "]" :
		connected	? BLUE( " (ConnSocket)") :
		filestream	? YELLOW( " (FileStream) ") :
		P			? PURPLE( " (Pipe)") : "") << endl;

}


// void	PollSet::dropLink(Stream* link)
// {
// 	Pipe*			linkPipe = CONVERT(link, Pipe);
// 	FileStream*		linkFile = CONVERT(link, FileStream);

// 	iterator		it = getIterator(link);
// 	iterator_p		itPoll = it.first;
// 	iterator_s		itLink = it.second;

// 	if (linkFile)	{ TAG(PollSet, dropLink); cout << GRAY("Destroy linked File ") << (*itLink)->getFD() << endl; }
// 	if (linkPipe)	{ TAG(PollSet, dropLink); cout << GRAY("Destroy linked pipe ") << (*itLink)->getFD() << endl; kill(linkPipe->pid, SIGKILL); }	//NOTE: cannot waitpid() here due to delay. we will check every child process later, because we don't want looping or blocking operation.

// 	(*itLink)->close();
// 	delete (*itLink);
// 	pollVec.erase(itPoll);
// 	streamVec.erase(itLink);
// }





void	PollSet::_drop( int fd )
{

	// Stream*	link1 = NULL;
	// Stream*	link2 = NULL;
	// Stream*	link3 = NULL;
	// Stream*	link4 = NULL;
	// Stream*	link5 = NULL;

	Poll	p;
	Stream* s;

	p = this->pollMap[fd].first;
	s = this->pollMap[fd].second;
	TAG(PollSet, drop); cout << GRAY("Drop ") << fd << " ";
	ConnSocket* connSock = CONVERT(s, ConnSocket);
	if (connSock)
	{
		cout << BLUE("(ConnSocket)") << endl;
		if (connSock->linkInputPipe)
		{
			// link1 = connSock->linkInputPipe;
			connSock->linkInputPipe->linkConn = NULL;
		}
		if (connSock->linkOutputPipe)
		{
			// link2 = connSock->linkOutputPipe;
			connSock->linkOutputPipe->linkConn = NULL;
		}
		if (connSock->linkErrorPipe)
		{
			// link3 = connSock->linkOutputPipe;
			connSock->linkErrorPipe->linkConn = NULL;
		}
		if (connSock->linkInputFile)
		{
			// link4 = connSock->linkInputFile;
			connSock->linkInputFile->linkConn = NULL;
		}
		if (connSock->linkOutputFile)
		{
			// link5 = connSock->linkOutputFile;
			connSock->linkOutputFile->linkConn = NULL;
		}
	}

	else if (CONVERT(s, Pipe))
 		cout << PURPLE("(Pipe)") << endl;

	else if (CONVERT(s, FileStream))
 		cout << YELLOW("(FileStream)") << endl;


	delete s;
	pollMap.erase(fd);
	close(fd);
}

void	PollSet::drop( Stream* stream )
{
	_drop(stream->getFD());
}

void	PollSet::drop( int fd )
{
	_drop(fd);
}

void	PollSet::makePollVec()
{

	_Map::iterator it, ite;
	it = pollMap.begin(), ite = pollMap.end();

	pollVec.clear();
	for ( ; it != ite; it++)
	{
		pollVec.push_back(it->second.first);
	}

}

vector<Stream*>	PollSet::examine()
{
	int		numReady = 0;
	time_t	minRemaining = 2000;;

	if (timer)
	{
		if ((minRemaining = getMinimumRemaining() * 1000) < 0)
			minRemaining = 2000;
		dropTimeout();
	}

	makePollVec();
	_Vp::iterator it, ite;
	vector<Stream*> ret;
	it = pollVec.begin(), ite = pollVec.end();

	switch (numReady = ::poll(pollVec.data(), pollVec.size(), minRemaining/*time-out*/))
	{
	case -1: TAG(PollSet, examine); cerr << RED("poll() ERROR: ")	<< strerror(errno) << endl;	break;
	case  0: TAG(PollSet, examine); cerr << GRAY("No event within ") << minRemaining << "ms" << endl;					break;
	default:;
	}

	for (; it < ite; it++)
	{
		if		(it->revents == 0)			continue;
		else
		{
			pollMap[it->fd].first.revents = it->revents;
			Stream* stream = pollMap[it->fd].second;
			//IMPL: separate updateLastWrite, updateLastRead for send_timeout
			pollMap[it->fd].second->updateLastActive();
			if 			(it->revents & POLLOUT)		ret.push_back( writeRoutine(stream) );
			else if		(it->revents & POLLIN)		ret.push_back( readRoutine(stream) );
		}
	}
	if (ret.empty())
		throw exception();
	return ret;
}

Stream*				PollSet::readRoutine(Stream* stream)
{
	ServerSocket*	serv		= CONVERT(stream, ServerSocket);
	ConnSocket*		connected	= CONVERT(stream, ConnSocket);
	Pipe*			CGIpipe		= CONVERT(stream, Pipe);
	FileStream*		filestream	= CONVERT(stream, FileStream);

	if (serv)
	{
		Poll p;
		p.fd = serv->getFD();
		while(1)	// accept() will throw exception if not readied
		{
			try
			{
				connected = new ConnSocket(serv->accept());
				TAG(PollSet, examine); cout << GREEN("Server ") << _UL "[" << serv->getIP() + ":" + toString(serv->getPort()) + _NC "]"<<  GREEN(" Got new connection, enroll ") << connected->getFD() << endl;
				this->enroll(connected, POLLIN);
			}
			catch (exception& e)	// accept() not ready
			{	continue;	}

			return (serv);
		}
	}
	else if (connected)
	{
		// TAG(PollSet, examine); cout << GREEN("New data to read ")
		// << stream->getFD() << BLUE(" (ConnSocket)") <<endl;
		return connected;
	}
	else if (CGIpipe)
	{
		// TAG(PollSet, examine); cout << GREEN("New data to read ")
		// << stream->getFD() << PURPLE(" (Pipe)") <<endl;
		return CGIpipe;
	}
	else if (filestream)
	{
		// TAG(PollSet, examine); cout << GREEN("New data to read ")
		// << stream->getFD() << YELLOW(" (FileStream)") <<endl;
		return filestream;
	}
	else
	{
		// TAG(PollSet, examine); cout << RED("Unknown type: ") << endl;
		throw exception();
	}
}

Stream*	PollSet::writeRoutine(Stream* stream)
{
	TAG(PollSet, examine); cout << GREEN("Can write to ") << stream->getFD() << endl;
	return stream;
}







const Poll&		PollSet::getPoll(const Stream* const stream) const				{ return _getPoll(stream->getFD()); }
const Poll&		PollSet::getPoll(int fd) const									{ return _getPoll(fd); }
const Poll&		PollSet::_getPoll(int fd) const									{ return pollMap.at(fd).first; }

void			PollSet::setEvent(const Stream* const stream, short event)		{ _setEvent(stream->getFD(), event); }
void			PollSet::setEvent(int fd, short event)							{ _setEvent(fd, event); }
void			PollSet::_setEvent(int fd, short event)							{ pollMap[fd].first.events |= event; }

void			PollSet::unsetEvent(const Stream* const stream, short event)	{ _unsetEvent(stream->getFD(), event); }
void			PollSet::unsetEvent(int fd, short event)						{ _unsetEvent(fd, event); }
void			PollSet::_unsetEvent(int fd, short event)						{ pollMap[fd].first.events &= ~event; }

void			PollSet::prepareSend( int fd )									{ _setEvent(fd, POLLOUT); }
void			PollSet::prepareSend( const Stream* const stream )				{ _setEvent(stream->getFD(), POLLOUT); }

void			PollSet::unsetSend( int fd )									{ _unsetEvent(fd, POLLOUT); }
void			PollSet::unsetSend( const Stream* const stream )				{ _unsetEvent(stream->getFD(), POLLOUT); }

short			PollSet::getCatchedEvent(const Stream* const stream) const		{ return _getPoll(stream->getFD()).revents; }
short			PollSet::getCatchedEvent(int fd) const							{ return _getPoll(fd).revents; }


void	PollSet::createMonitor() { this->timer = new Timer(); }
time_t	PollSet::getMinimumRemaining()
{
	timer->monitor();
	if (timer->min.first == -1)
		return -1;
	else
	 	return timer->min.second;
}

void	PollSet::dropTimeout()
{
	vector<int>::iterator it	= timer->timeoutPool.begin();
	vector<int>::iterator ite	= timer->timeoutPool.end();

	for (; it < ite; it++)
	{
		//IMPL: reset_timedout_connection
		// struct linger l = {.l_onoff = 1, .l_linger = 0};
		// setsockopt((*itStream)->getFD(), SOL_SOCKET, SO_LINGER, &l, sizeof(l));
		POLLSET->drop(*it);
	}
	timer->timeoutPool.clear();
}


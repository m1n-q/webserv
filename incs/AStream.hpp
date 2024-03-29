#ifndef STREAM_HPP
# define STREAM_HPP

#include <ctime>
#include <map>
# include <string>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

class Poll;
class AStream
{
/**========================================================================
* %                          member variables
*========================================================================**/

protected:
	int		fd;
	time_t	timeout;
	time_t	lastActive;

public:
	Poll*	p;

/**========================================================================
* @                           Constructors
*========================================================================**/

public:
	AStream();
	AStream( int _fd );
	AStream( int _fd, time_t _to);
	AStream( const AStream& src );
	virtual ~AStream();

/**========================================================================
* *                            operators
*========================================================================**/

	AStream&	operator=( const AStream& src );

/**========================================================================
* #                          member functions
*========================================================================**/

	int				getFD()	const;
	void			setFD( int fd );
	void			updateLastActive();
	time_t			getLastActive() const;
	void			setTimeOut(time_t to);
	time_t			getTimeOut() const;
	void			close();

/**========================================================================
* !                            Exceptions
*========================================================================**/

	class somethingWrong: public exception
	{
		private:	string msg;
		public:		explicit somethingWrong(const string& m): msg(m) {}
					virtual ~somethingWrong() throw() {};
					virtual const char * what() const throw() { return msg.c_str(); }
	};

private:
	virtual void	dummy() = 0;
public:
	virtual void	send(const string& content) = 0;
	virtual void	recv() = 0;
	virtual void	coreDone() = 0;
	virtual void	core() = 0;
	virtual string	getOutputContent() = 0;

};





#endif

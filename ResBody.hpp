#ifndef RESBODY_HPP
# define RESBODY_HPP

#include "ConnSocket.hpp"
#include "IText.hpp"
#include "color.hpp"
#include <iostream>
# include <string>
#include <sys/errno.h>
# include <sys/fcntl.h>
# include <unistd.h>
# include <fstream>
using namespace std;

class ResBody : public IText
{
public:

	ResBody(): IText() {}
	ResBody( const string& path ): IText()
	{
		readFile(path);
	}
	ResBody( const ResBody& src ): IText(src.content) {}
	~ResBody() {}

	ResBody&	operator=( const ResBody& src )
	{
		if (this != &src)
			content = src.content;
		return *this;
	}

	status_code_t	readFile( const string& path )
	{
		ifstream input_file(path);

		if (!input_file.is_open())
		{
			int	fd;
			errno = 0;
			fd = open(path.c_str(), O_RDONLY);
			if (fd == -1)
				TAG(ResBody, readFile) << RED("open Fail: ") << path << " => " << strerror(errno) << endl;
			else
				close(fd);	// NOREACH
		}
		else
    		content = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		switch (errno)
		{
		case 0:			return 200;
		case ENOENT:	return 404;
		default:		return 200;
		}
	}

	void	clear() { content.clear(); /*IText::clear();*/ }
};
#endif

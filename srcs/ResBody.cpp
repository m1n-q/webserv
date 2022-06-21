#include "ResBody.hpp"
#include "FileStream.hpp"



/**========================================================================
* *                            operators
*========================================================================**/

	ResBody&	ResBody::operator=( const ResBody& src )
	{
		if (this != &src)
			content = src.content;
		return *this;
	}

/**========================================================================
* #                          member functions
*========================================================================**/

	status_code_t	ResBody::readFile( const string& path )
	{
		errno = 0;
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
		case ENOENT:     return 404;
		default:		return 500;
		}
	}

	status_code_t	ResBody::readFile( FileStream* f )
	{
		ssize_t	byte;
		byte = f->read();
		if (byte != -1)
			this->content.append(f->content);
		return 200;
	}

	void	ResBody::clear() { content.clear(); /*IText::clear();*/ }


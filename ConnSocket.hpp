/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnSocket.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishin <mishin@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/06 12:03:16 by mishin            #+#    #+#             */
/*   Updated: 2022/05/11 00:15:32 by mishin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNSOCKET_HPP
# define CONNSOCKET_HPP

# include <iostream>
#include <string>
#include <sys/_types/_ssize_t.h>
# include <utility>

# include <sys/fcntl.h>

# include "ISocket.hpp"
# include "Config.hpp"

# include "utils.hpp"
# include "ResHeader.hpp"

# define GET	1
# define PUT	2
# define POST	3
# define DELETE	4


/*
 @ There are 3 ways of detecting the end of the stream depending on what requests you are handling:
 * If it is a GET or HEAD request, you only need to read the HTTP headers, request body is normally ignored if it exists, so when you encounter \r\n\r\n, you reach the end of the request(actually the request headers).
 * If it is a POST method, read the Content-Length in the header and read up to Content-Length bytes.
 * If it is a POST method and the Content-Length header is absent, which is most likely to happen, read until -1 is returned, which is the signal of EOF.
*/

typedef unsigned short status_code_t;

class ConnSocket : public ISocket
{
friend class ServerSocket;

private:
	socklen_t	len;
	char		recvbuf[1024];
	// char		sendbuf[1024];	//' is required?

public:
	ConnSocket() : ISocket(), len(sizeof(info)) {}
	~ConnSocket() {}

	ConnSocket&	operator=( const ConnSocket& src )
	{
		if (this != &src)
		{
			this->ISocket::operator=(src);
			this->len		= src.len;
			//NOTE: no buf copy
		}
		return *this;
	}

	char checkMethod(const string& src)
	{
		string::size_type end = src.find(" ");
		string method = src.substr(0, end);
		if		(method == "GET")	return GET;
		else if (method == "PUT")	return PUT;
		else if (method == "POST")	return POST;
		else if (method == "DELETE")return DELETE;
		return -1;
	}

	ReqHeader recvRequest()
	{
		ssize_t	byte;
		string	content;
		int		method = 0;
		bzero(recvbuf, sizeof(recvbuf));
		while ( (byte = read(this->sock, this->recvbuf, sizeof(recvbuf))) > 0)
		{
			content += recvbuf;	//NOTE: if read binary from req?
			if (!method) method = checkMethod(content);
			switch (method)
			{
			case GET:
				if (content.substr(content.length() - 4) == "\r\n\r\n")
					goto exitloop;		// ' 'break' do not exit the loop due to switch
										// ! read() will be blocked until EOF, need 'break'
				break;
			case PUT:	cerr << "NOT SUPPORT PUT" << endl;		break;
			case POST:	cerr << "NOT SUPPORT POST" << endl;		break;
			case DELETE:cerr << "NOT SUPPORT DELETE" << endl;	break;
			}
			bzero(recvbuf, sizeof(recvbuf));
		}
	exitloop:
		if (byte == -1)	throw something_wrong(strerror(errno));

		ReqHeader req(content);
		cout << req.getContent() << endl;
		return req;
	}

	void	send(const string& content)
	{
		write(this->sock, content.data(), content.length());
		// write(STDOUT_FILENO, content.data(), content.length());
	}

	void	sendResponseBody()
	{
		;
		// ssize_t	byte = 0;
		// bzero(sendbuf, sizeof(sendbuf));
		// while ( resbody.content -> sendbuf )
		// {
		// 	write(this->sock, sendbuf, byte);
		// 	// bzero(sendbuf, sizeof(sendbuf));
		// }
	}

	void	sendResponse()
	{
		;
		// pair<int, status_code_t>	fd_status	= checkFile();
		// sendResponseHeader( makeResponseHeader(fd_status.second) );
		// sendResponseBody();
	}
private:
	void			dummy() {}
};




#endif

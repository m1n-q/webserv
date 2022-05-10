/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReqHeader.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishin <mishin@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/09 18:30:30 by mishin            #+#    #+#             */
/*   Updated: 2022/05/10 22:10:07 by mishin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQHEADER_HPP
# define REQHEADER_HPP
# include "IHeader.hpp"
# include "Config.hpp"
# include <fcntl.h>
#include <string>

class ReqHeader : public IHeader
{
private:
	typedef unsigned short status_code_t;


public:
	string			reason;
	status_code_t	status;
	string			path;

	ReqHeader() : IHeader() {}
	ReqHeader( const string& c ) : IHeader(c) {}
	ReqHeader( const ReqHeader& src ): IHeader(src) {}	//TODO
	~ReqHeader() {}

	ReqHeader&	operator=( const ReqHeader& src )
	{
		if (this != &src)
			this->IHeader::operator=(src);
		return *this;
	}

	void	setPath()
	{
		string::size_type start = content.find(" ") + 1;
		string::size_type end	= content.find(" ", start);

		this->path = root + content.substr(start, end - (start));
	}

	void	checkFile()	// set (status code,  reason)
	{
		int	requested	= open(path.c_str(), O_RDONLY);
		if (requested == -1)
		{
			if (errno == ENOENT) { this->status = 404; this->reason = "File Not Found"; }
			// . . .
			return ;
		}
		close(requested);
		this->status = 200;
		this->reason = "OK";
	}
	/*
	 .	separate getStatus() / getReason() ?
	 	switch (req.status)
		{
		case 200:	req.reason = "OK"; break;
		case 404:	req.reason = "NOT FOUND"; break;
		}
	*/

	void	clear()
	{
		content = reason = path = "";
		status = -1;
	}

};
#endif

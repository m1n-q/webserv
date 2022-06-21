#include "CGI.hpp"
#include "Config.hpp"
#include "ConnSocket.hpp"
#include "Poll.hpp"
# include "ResBody.hpp"
# include "ResHeader.hpp"
#include "utils.hpp"

// string	        parseFileName(ConnSocket* connected, const string& reqTarget);
status_code_t	writeResponseBody(ConnSocket* connected, const string& filepath, pair<string, string> p);
status_code_t	createPutFile(ConnSocket* connected, pair<string, string> p);
status_code_t	deleteFile(pair<string, string> p);
void			writeResponseHeader(ConnSocket* connected);
void			core(PollSet& pollset, ServerSocket *serv, ConnSocket *connected);
void			core_wrapper(PollSet& pollset, ServerSocket *serv, ConnSocket *connected, Pipe* CGIpipe);


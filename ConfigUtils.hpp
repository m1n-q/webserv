#ifndef CONFIGUTILS_HPP
#define CONFIGUTILS_HPP
#include "Config.hpp"
#include "ServerConfig.hpp"
// class Config;
class HttpConfig;
class ServerConfig;
class LocationConfig;

using namespace std;

//=====================================parse_util========================================
void    isPath(string path)
{
    if (path.find("$") != string::npos)
        throw Config::parseFail();
}

string    convertStringToLower(string str)
{
    string  lower = "";

    for (string::size_type i = 0; i < str.length(); i++)
        lower.append(1, tolower(str[i]));
    return lower;
}

string    convertStringToUpper(string str)
{
    string  upper = "";

    for (string::size_type i = 0; i < str.length(); i++)
        upper.append(1, toupper(str[i]));
    return upper;
}

ssize_t    convertStringToSsize_T(string val)
{
    ssize_t         num = 0;
    istringstream   iss;

    if (val.find_first_not_of("0123456789") != string::npos)
        throw Config::parseFail();
    iss.str(val);
    iss >> num;
    if (iss.fail())
        throw Config::parseFail();
    return num;
}

int    convertStringToStateCode(string code)
{
    ssize_t         val = 0;

    val = convertStringToSsize_T(code);
    if (!(300 <= val && val < 600))
        throw Config::parseFail();
    return static_cast<int>(val);
}

string  convertStringToIpv4(unsigned int ip)
{
    stringstream    ss;
    string          hexString;
    string          cut;
    int             pos;
    string          ipv4 = "";
    int             decimal;

    ss << hex << ip;
    hexString = ss.str();
    pos = static_cast<int>(hexString.length());
    for (int i = 0; i < 4; i++)
    {
        ss.str("");
        pos -= 2;
        if (pos == -1)
            decimal = strtol(hexString.c_str(), NULL, 16);
        else if (pos >= 0)
        {
            cut = hexString.substr(pos, 2);
            cout << cut << endl;
            decimal = strtol(cut.c_str(), NULL, 16);
        }
        else
            decimal = 0;       
        ss << "." << dec << decimal;
        ipv4.insert(0, ss.str());
    }
    ipv4.erase(0, 1);
    return ipv4;
}

string  convertStringToIP(string ip)
{
    ssize_t numString;
    int     dot;

    if ((ip.find(".")) != string::npos)
    {
        dot = 0;
        for (string::size_type i = 0; i < ip.length(); i++)
        {
            if (ip[i] == '.')
            dot++;
        }
        if (inet_addr(ip.c_str()) != (in_addr_t)-1)
        {
            for(dot = 4 - dot; dot > 0; dot--)
                ip.insert(0, "0.");
            return ip;
        }
        throw Config::parseFail();
    }

    numString = convertStringToSsize_T(ip);
    return convertStringToIpv4(static_cast<unsigned int>(numString));
}

unsigned short  convertStringToPort(string code)
{
    ssize_t         val = 0;

    val = convertStringToSsize_T(code);
    if (!(0 <= val && val <= 65535))
        throw Config::parseFail();
    return static_cast<unsigned short>(val);
}

pair<ssize_t, string>    splitStringToType(string val)
{
    string::size_type   pos;
    string              numStr = "";
    string              type = "";
    ssize_t             num;
    istringstream       iss;

    pos = val.find_first_not_of("0123456789", 0);
    if (pos != string::npos)
    {
        type = val.substr(pos, val.length());
        numStr = val.substr(0, pos);
    }
    else
        numStr = val.substr(0, val.length());

    iss.str(numStr);
    iss >> num;
    if (iss.fail())
        throw Config::parseFail();
    return make_pair(num, type);
}

time_t   convertStringToTime(string val)
{
    int                     pos;
    time_t                  numTime;
    string                  timeType[4] = {"h", "m", "s", "ms"};
    pair<ssize_t, string>   valuePair;

    valuePair = splitStringToType(val);
    if (valuePair.second == "")
        valuePair.second = "ms";
    numTime = static_cast<time_t>(valuePair.first);
    
    for (pos = 0; pos < 4; pos++)
    {
        if (timeType[pos] == valuePair.second)
            break;
    }
    switch (pos)
    {
    case 0:
        if (numTime > 2562047788)
            throw Config::parseFail();
        numTime *= 60;
    case 1:
        if (numTime > 153722867280)
            throw Config::parseFail();
        numTime *= 60;
    case 2:
        if (numTime > 9223372036854)
            throw Config::parseFail();
        numTime *= 1000;
    case 3:
        break;
    case 4:
        throw Config::parseFail();
    }
    return numTime;
}

ssize_t   convertStringToByte(string val)
{
    char                    type;
    ssize_t                 numByte;
    pair<ssize_t, string>   valuePair;

    valuePair = splitStringToType(val);
    if (valuePair.second.length() > 1 || valuePair.second.find_first_not_of("gmkGMK", 0) != string::npos)
        throw Config::parseFail();
    numByte = valuePair.first;

    type = *(valuePair.second.c_str());
    switch (tolower(type))
    {
    case 'g':
        if (numByte > 8589934591)
            throw Config::parseFail();
        numByte *= 1024;
    case 'm':
        if (numByte > 8796093022207)
            throw Config::parseFail();
        numByte *= 1024;
    case 'k':
        if (numByte > 9007199254740991)
            throw Config::parseFail();
        numByte *= 1024;
        break;
    }
    return numByte;
}
//=====================================parse_util========================================


//=====================================parse_arg========================================

void parse_root(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    isPath(arg[0]);
    config->root = arg[0];

    // for (size_t i = 0; i < arg.size(); i++)
    //     cout << arg[i] << " ";
    // cout << endl;
    // cout << GREEN("root success") << endl;
    // cout << GREEN("") << endl;
    // cout << typeid(config).name() << endl;
    // // HttpConfig *http = dynamic_cast<HttpConfig *>(config);
    // ServerConfig *server = dynamic_cast<ServerConfig *>(config);
    // // LocationConfig *location = dynamic_cast<LocationConfig *>(config);

    // // cout << typeid(http).name() << endl;
    // cout << typeid(server).name() << endl;
    // // cout << typeid(location).name() << endl;

    // cout << GREEN("") << endl;
    // throw Config::parseFail();
}

void parse_listen(vector<string> arg, Config *config)
{
    string::size_type   pos;

    if (arg.size() != 1)
        throw Config::parseFail();
    ServerConfig * server = dynamic_cast<ServerConfig *>(config);
    if ((pos = arg[0].find(':')) != string::npos)
    {
        server->ip.push_back(convertStringToIP(arg[0].substr(0, pos)));
        server->port.push_back(convertStringToPort(arg[0].substr(pos, arg[0].length())));
    }
    server->port.push_back(convertStringToPort(arg[0]));

    // for (size_t i = 0; i < arg.size(); i++)
    //     cout << arg[i] << " ";
    // cout << endl;
    // cout << GREEN("listen success") << endl;
    // cout << typeid(config).name() << endl;
}

void parse_server_name(vector<string> arg, Config *config)
{
    ServerConfig * server = dynamic_cast<ServerConfig *>(config);
    
    for (vector<string>::size_type i = 0; i < arg.size(); i++)
        server->server_name.push_back(arg[i]);

    // for (size_t i = 0; i < arg.size(); i++)
    //     cout << arg[i] << " ";
    // cout << endl;
    // cout << GREEN("server_name success") << endl;
    // cout << typeid(config).name() << endl;
}

void parse_index(vector<string> arg, Config *config)
{
    for (vector<string>::size_type i = 0; i < arg.size(); i++)
    {
        isPath(arg[i]);
        config->index.push_back(arg[i]);
    }

    // for (size_t i = 0; i < arg.size(); i++)
    //     cout << arg[i] << " ";
    // cout << endl;
    // cout << GREEN("index success") << endl;
    // cout << typeid(config).name() << endl;
}

void parse_auto_index(vector<string> arg, Config *config)
{
    string  button;

    if (arg.size() != 1)
        throw Config::parseFail();
    button = convertStringToLower(arg[0]);
    if (button == "off")
        config->auto_index = 0;
    else
        config->auto_index = 1;
}

void parse_error_page(vector<string> arg, Config *config)
{
    string  path;
    int     status;

    if (arg.size() < 2)
        throw Config::parseFail();
    isPath(arg[arg.size() - 1]);
    path = arg[arg.size() - 1];

    for (vector<string>::size_type i = 0; i < (arg.size() - 1); i++)
    {
        status = convertStringToStateCode(arg[i]);
        config->error_page[status] = path;
    }
}

void parse_keepalive_requests(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->keepalive_requests = convertStringToSsize_T(arg[0]);
}

void parse_default_type(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->default_type = arg[0];
}

void parse_client_body_size(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->client_body_size = convertStringToByte(arg[0]);
}

void parse_reset_timedout_connection(vector<string> arg, Config *config)
{
    string  button;

    if (arg.size() != 1)
        throw Config::parseFail();
    button = convertStringToLower(arg[0]);
    if (button == "off")
        config->reset_timedout_connection = 0;
    else
        config->reset_timedout_connection = 1;
}

void parse_lingering_timeout(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->lingering_timeout = convertStringToTime(arg[0]);
}

void parse_lingering_time(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->lingering_time = convertStringToTime(arg[0]);
}

void parse_keepalive_time(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->keepalive_time = convertStringToTime(arg[0]);
}

void parse_keepalive_timeout(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->keepalive_timeout = convertStringToTime(arg[0]);
}

void parse_send_timeout(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->send_timeout = convertStringToTime(arg[0]);
}

void parse_client_body_timeout(vector<string> arg, Config *config)
{
    if (arg.size() != 1)
        throw Config::parseFail();
    config->client_body_timeout = convertStringToTime(arg[0]);
}

void parse_location(string uri, Config *config)
{
    LocationConfig  *location;

    if (uri.length() == 0)
        throw Config::parseFail();
    location = dynamic_cast<LocationConfig *>(config);
    if (uri[0] == '=')
    {
        location->assign = 1;
        uri.erase(0, 1);
    }
    location->URI = uri;
}

void parse_limit_except_method(vector<string> arg, Config *config)
{
    LocationConfig              *location;
    string                      method;
    vector<string>::size_type   check;

    location = dynamic_cast<LocationConfig *>(config);
    for (vector<string>::size_type i = 0; i < arg.size(); i++)
    {
        method = convertStringToUpper(arg[i]);
        check = 0;
        for (check = 0; check < location->limit_except_method.size(); check++)
        {
            if (location->limit_except_method[check] == method)
                break;
        }
        if (check == location->limit_except_method.size() || method == "GET" || method == "POST" || method == "PUT" || method == "DELETE")
            location->limit_except_method.push_back(method);
    }
}

//=====================================parse_arg========================================


void EraseComment(string &configtemp)
{
    size_t start = 0;
    size_t end = 0;

    while ((start = configtemp.find('#')) != string::npos)
    {
        end = configtemp.find('\n', start);
        configtemp.erase(start, end - start);
    }
}

string ReadConfig(char **argv)
{
    string line, configtemp;
    ifstream file(*(argv + 1));

    if (file.is_open())
    {
        while (getline(file, line))
            if (line.length() != 0)
                configtemp += line + '\n';
        file.close();
    }
    else
        cout << "Unable to open file"; // throw

    EraseComment(configtemp);
    return (configtemp);
}
#endif

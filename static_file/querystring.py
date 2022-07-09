#!/usr/local/bin/python3
print("Content-Type: text/html")
print()

import cgi
form = cgi.FieldStorage()  
pageId = form["id"].value  

print('''<!DOCTYPE html>
<html>
<head>
<title>Welcome to {title}!</title>
</head>
<body>
<h1>Welcome to {title}!</h1>
<p>If you see this page, the {title} web server is successfully installed and
working. Further configuration is required.</p>

<p><em>Thank you for using {title}.</em></p>
</body>
</html>
'''.format(title = pageId))

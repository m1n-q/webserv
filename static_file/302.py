#!/usr/local/bin/python3
import cgi
import cgitb
#. client redir

#% server redir

print("Status: 200 Found", flush=True)
print("Content-type: text/htmlss", flush=True)
print("Accept: text/abcd", flush=True)
print("Location: http://google.com", flush=True)

print(flush=True)

# @OK
# %NOT YET

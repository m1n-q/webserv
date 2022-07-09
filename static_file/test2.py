#!/usr/local/bin/python3
import cgi
import cgitb
import time
import os

#. client redir
# without body:
#	lighttpd : test.py=302 dummy.txt=200
#	apache : test.py=302 dummy.txt=200

	# if other header exists,
	# 	lighttpd : add to dummy.txt response header
	# 	apache : add to dummy.txt response header

# with body:
#	lighttpd : test.py=302 dummy.txt=200, discard body
#	apache : test.py=302 dummy.txt=200, discard body
# print("Location: http://localhost:8080/es.png")

#% server redir

# without body:
#	lighttpd : test.py=302 dummy.txt=200 (client redir to dummy.txt)
#	apache: test.py = 200
	# if other header exists,
	# 	lighttpd : add to test.py response header, client redir to dummy.txt
	# 	apache : add to test.py (response of dummy.txt) response header

# with body:
#	lighttpd: test.py = 302, dummy.txt = 200, discard body
#	apache: only test.py = 200, discard body


# print("Location:  /es.png")
print("Status: 200 Found", flush=True)
print("Content-type: text/htmlss", flush=True)
# time.sleep(3)
print("Accept: text/abcd", flush=True)
# time.sleep(3)
# print("Location: /directory/nop/other.pouic", flush=True)
print("Location: /localredir/test3.py", flush=True)
print(flush=True)
# time.sleep(3)

# print("Content-length: 2022")
# time.sleep(3)
# time.sleep(1)
# time.sleep(3)
# print("Transfer-Encoding: chunked")

# print("Transfer-Encoding: gzip")
# print("Content-length: 2022")


# print("Hello world")

for i in range(10):
	time.sleep(1)
	# time.sleep(1)
	print(i, flush=True)
cgi.print_environ()






# @OK
# %NOT YET

# if Location:
#	if Server redirect:
#		Content-type and Content-length from script replaced. (other can be replaced. need to inspect)
#		other field from script added. (ex/ Accept)

#	elif client redirect:
#		test.py =>	#@ Content-type is replaced to : text/html; charset=iso-8859-1
#					#@ Content-Length is replaced to : 212 (302 docs)

#					Content-Range is removed if  status is not 206 / 416  ( in apache )
#					Content-Range is not removed, and multiple fields allowd ( in lighttpd )

					# ' No Multiple (last value)			: Content-type, Status, Location, Content-Length, Content-Range, Transfer-Encoding, ETag
					# * Allow Multiple (Append new value)	: anything else

					# Last-Modified ->  If the script gave us a Last-Modified header, we can't just pass it on blindly because of restrictions on future values.
					# Date, Keep-Alive, Server -> following server config
					# Connection is little bit confusing
#					other fields like Accept are not replaced.
#		es.png	=> it's own response.


servermake:
	gcc FTPServer.c  -o FTPserver
	gcc FTPClient.c  -o FTPclient
clean:
	rm FTPserver FTPclient
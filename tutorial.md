1.1 FTP commands
FTP RFC 959 details a large number of commands that the FTP protocol, in this project we are only going to implement a couple of these commands. Below is a list of all the required commands that you need to implement.
USER username with this command the client gets to identify which user is trying to login to the FTP server. This is mainly used for authentication, since there might be different access controls specified for each user.
PASS password once the client issues a USER command to the server and gets the ”331 Username OK, password required” status code from the server. The client needs to authen- ticate with the user password by issuing a PASS command followed by the user password.
PUT filename this command is used to upload a file named filename from the current client directory to the current server directory. This command should trigger the transfer of the file all the way to the server. Remember that a separate TCP connection to the server needs to be opened for the data transfer.
GET filename this command is used to download a file named filename from the cur- rent server directory to the current client directory. Similar to PUT, a new separate TCP connection to the server needs to be opened for the data transfer.
LS this command is used to list all the files under the current server directory !LS this command is used to list all the files under the current client directory CD this command is used to change the current server directory
!CD this command is used to change the current client directory
PWD this command displays the current server directory !PWD this command displays the current client directory
2
QUIT this command quits the FTP session and closes the control TCP connection

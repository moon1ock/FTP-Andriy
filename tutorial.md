# This is a simple tutorial for the FTP program that I created

#### the program works best if you restart your terminal, so please make sure to do it
#### before running the makefile

## Login

   I decided to have all the commands processed on the server side in order to learn
   more about security and not let the potential client abuse the system (by accessing)
   the login/password file, for example.


   Logins and passwords are stored in the db.txt file one by one in a form login'\n'password
   This was done to ease my life with parsing, I was keeping in mind that it is not a computer
   security class.

   Once client connects to the server, it is prompted to login: `USER Yasir` for example.
   Once the login sent to the server, server replies, telling the client that the username
   was found, and that the server expects a password. Once the correct password is entered,
   all the other functions become available for the client.

   Since I am processing these quieries from the server side, I paid extra attention to parsing,
   and did my best not to let users abuse the login process or cause any errors. You may check that
   commands that don't have a space after `USER` or immediately terminate like `USER\t` or `USER\n`
   do not cause errors.


## Rest of the commands

   The `LS`, `CD`, `PWD`, are rather starighforward.

   For the `PUT` / `GET` command, I do open a new TCP connection between the client and the server,
   do some error checking, as in, I avoid files with the same filenames, not to cause errors.
   If the command executed correctly, the file is split in chunks of 1024 bytes, and sent to the
   server, with expected confirmation after each packet.

   The biggest filesize that I tried was 3.7MB (you can find it in the server folder: c.txt),
   and it worked just great.


---------------

### On aggregate, I have to admit that I did overestimate myself at first, and the lab turned out to
### be more challanging than I expected, but I did really enjoy it, and can surely say that I learned
### a lot. Please contact me if you have any questions about the functions / programs.



also, i would greatly appreciate if you could explain me how to split the .c file into headers, and
separate files, as I gave up trying to do it on my own, TIA!

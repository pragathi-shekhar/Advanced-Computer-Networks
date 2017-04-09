Steps for Execution
===================
#### Make sure the server and client are in the same network or the server is accessible to the client via a valid hostname.

For server
-----------
### - Execute the following commands.  
gcc server.c -o server -l pthread;  
./server  
```

For Client
-----------
### - Execute the following commands.   
gcc client.c -o client
./client <IP adress>  
```

### - Execute 
```./client <server IP/Hostname>```
the ```<server IP/Hostname>``` is the name or IP address of the server where the server side code is running.

### Note: Start the server before the client.
## The remaining part of the execution will be suggested by the program itself. 




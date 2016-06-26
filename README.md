#Hooli Drive 
 
This repository contains my client and server implementations of a personal cloud storage service (sometimes referred to as an online backup service) that can be used used for file sharing and collaboration.  
 
When the server and client programs are run simultaneously the client's specified directory will be synced with the server's. This happens by first using /HMDS to send the file names and checksums then comparing with the server to find changed files. After this the changed files are transmitted in blocks and reconstructed on the server side
 
##About Development 
This project was developed to allow me to obtain more hands-on knowledge with the layers of networking. 
I read 'Networking. A top-down approach' and experimented with UDP and TCP.  
 
The file names and checksum is sent using TCP within /HMDS because this data needs reliable data transfer and isnt expected to be large. The file data itself is sent using protocol i designed on top of UDP which ensures reliable data transfer while taking advantage of the speed benefit of UDP. 
 
Overall this was a very good learning experience for me.

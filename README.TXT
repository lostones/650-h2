README.TXT

- For this project, I needed to create a /tmp/mab99/ folder everytime I logged onto the teer linux machine. the reason is that when I tried to create fifos in my local directory, I was given an error message by the system that I dd not have the permission to do so.

- Need to run 'make clean' inbetween each test

- I noticed some erroneous behaviour of the program when running it for the first time with a certain number of players; I think that sometimes it requires you to run the program twice, once to set up all of the fifos, and once to actually play the game. 

- Does not work perfectly; was able to get it most of the way, however ran into issues trying to get it to pass from player to player

- this program does correctly establish all of the fifos in the /tmp/mab99/directory


- Open Items:
	- finish the unpacking function for a passed potato string
	- add sendPotato to inside of while loop in player
	- when hops == total number of hops, send back to master
	- master ends game and prints trace




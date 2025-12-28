# GameScore
 Project to demonstrate Multithreading using Pthreads.
• Each updater thread randomly selects a player and increments their 
score. 
• Mutex (pthread_mutex_t) ensures thread-safe access to shared 
variables. 
• The display thread periodically prints current scores. 
• The input thread handles real-time user input. 
• A timer thread stops the game after a fixed duration. 
• All updates are written to score.txt using file I/O. 


The scoreboard.c file contains the logic for the backend.
gui.pyw is the Python file for the frontend.
scores.txt logs the scores of each player.

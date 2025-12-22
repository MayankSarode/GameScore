import tkinter as tk
from tkinter import messagebox
import subprocess
import threading
import time
import os
import signal

process = None
running = False

def read_scores():
    while running:
        if os.path.exists("scores.txt"):
            with open("scores.txt", "r") as f:
                lines = f.readlines()
            for i in range(min(len(lines), len(labels))):
                score = lines[i].strip()
                labels[i].config(text=f"Player {i+1}: {score}")
        time.sleep(1)

def start_program():
    global process, running
    if running:
        messagebox.showinfo("Info", "Scoreboard is already running!")
        return
    try:
        # Start the multithreaded C scoreboard program
        process = subprocess.Popen(["scoreboard.exe"], creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)
        running = True
        messagebox.showinfo("Info", "Game Scoreboard started!")
        # Start background thread for updates
        threading.Thread(target=read_scores, daemon=True).start()
    except Exception as e:
        messagebox.showerror("Error", f"Failed to start: {e}")

def stop_program():
    global process, running
    if not running:
        messagebox.showinfo("Info", "Scoreboard is not running!")
        return
    try:
        running = False
        if process:
            os.kill(process.pid, signal.CTRL_BREAK_EVENT)
            process.wait(timeout=2)
        messagebox.showinfo("Info", "Game Scoreboard stopped!")
    except Exception as e:
        messagebox.showerror("Error", f"Failed to stop: {e}")

# GUI setup
root = tk.Tk()
root.title("Gaming Scoreboard")
root.geometry("400x400")
root.resizable(False, False)

title_label = tk.Label(root, text="Gaming Scoreboard", font=("Helvetica", 16, "bold"))
title_label.pack(pady=10)

labels = []
for i in range(5):
    lbl = tk.Label(root, text=f"Player {i+1}: 0", font=("Helvetica", 14))
    lbl.pack(pady=5)
    labels.append(lbl)

start_btn = tk.Button(root, text="Start Game", font=("Helvetica", 12), bg="green", fg="white", command=start_program)
start_btn.pack(pady=10)

stop_btn = tk.Button(root, text="Stop Game", font=("Helvetica", 12), bg="red", fg="white", command=stop_program)
stop_btn.pack(pady=10)

exit_btn = tk.Button(root, text="Exit", font=("Helvetica", 12), bg="gray", fg="white", command=root.destroy)
exit_btn.pack(pady=10)

root.mainloop()

import tkinter as tk
from tkinter import messagebox
import subprocess
import threading
import time
import os
import signal
import sys
process = None
running = False

if getattr(sys, 'frozen', False):
    # Running as PyInstaller EXE
    BASE_DIR = os.path.dirname(sys.executable)
else:
    # Running as normal .py / .pyw
    BASE_DIR = os.path.dirname(os.path.abspath(__file__))

scoreboard_path = os.path.join(BASE_DIR, "scoreboard.exe")
scores_path = os.path.join(BASE_DIR, "scores.txt")


def read_scores():
    while running:
        if os.path.exists(scores_path):
            with open(scores_path, "r") as f:
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
        process = subprocess.Popen([scoreboard_path], creationflags=subprocess.CREATE_NO_WINDOW)
        running = True
        messagebox.showinfo("Info", "Game Scoreboard started!")
        # Start background thread for updates
        threading.Thread(target=read_scores, daemon=True).start()
    except Exception as e:
        messagebox.showerror("Error", f"Failed to start: {e}")

def stop_program():
    global process, running

    if not running or process is None:
        messagebox.showinfo("Info", "Scoreboard is not running!")
        return

    try:
        if process.poll() is None:
            process.terminate()    
            process.wait(timeout=2)

        process = None
        running = False
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

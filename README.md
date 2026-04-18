# 🚗 EV Safety System using QNX RTOS

## 📌 Overview
This project implements a real-time automotive safety system using QNX RTOS. It integrates multiple subsystems such as Driver Monitoring System (DMS), Battery & Motor Monitoring, and Obstacle Detection to ensure safe vehicle operation. A central control process collects events from all subsystems and makes priority-based decisions in real time.

## 🧠 System Architecture
DMS Process, Battery Process, and Obstacle Process run independently and communicate with a central Control Process using QNX message passing. The Control Process evaluates all incoming events and executes the highest-priority action.0

## ⚙️ Tech Stack
- C (QNX RTOS) – Real-time backend
- POSIX Threads – Multithreading
- QNX Message Passing – IPC (MsgSend, MsgReceive)
- Shared Memory IPC

## 🔴 Features
Driver Monitoring System detects drowsiness using Eye Aspect Ratio and detects driver absence/fainting. It triggers a buzzer for drowsiness and activates autopilot in critical cases.

Battery & Motor Monitoring tracks battery current, temperature, and motor current. It handles overcurrent (shutdown), overtemperature (power reduction), and motor faults (torque limiting).

Obstacle Detection identifies obstacles and classifies them into safe, warning, or critical states. Critical obstacles trigger emergency braking.

The Control Process receives all subsystem events and ensures that the most critical event is handled first using priority-based decision logic.

## 🔁 Inter-Process Communication
All subsystems communicate with the control process using QNX IPC:
MsgSend() and MsgReceive()

Message structure used:
typedef struct {
    int source;
    int event;
    int priority;
} system_msg_t;

## 🔥 Priority Levels
Priority 4 – Critical (Driver fainted)  
Priority 3 – High (Obstacle, Overcurrent)  
Priority 2 – Medium (Drowsy, warnings)  
Priority 1 – Low (Normal monitoring)

## 🚨 Decision Logic
When multiple events occur, the control process stores all incoming events, compares their priorities, and executes the highest-priority action.

Example:
DMS → DROWSY (P2)  
Battery → OVERCURRENT (P4)  
Obstacle → WARNING (P3)  

Final Action → SHUTDOWN

## 🧵 Thread Architecture
Each subsystem uses multithreading:
DMS → Capture, Processing, Decision threads  
Battery → Sensor, Analysis, Safety threads  
Obstacle → Capture, Detection, Decision threads  

## ⚡ Real-Time Concepts Used
Preemptive scheduling, priority-based execution, deterministic timing, mutex for shared memory protection, condition variables, and message passing.

## ▶️ How to Run
1. Build the project in QNX Momentics.
2. Run the control process.
3. Run all subsystem processes (DMS, Battery, Obstacle).
   
## 📂 Project Structure
/project-root  
├── control_process.c  
├── sensor.c
└── README.md  

## 🎯 Design Justification
QNX RTOS is used for deterministic real-time performance, reliable inter-process communication, and fault isolation. Priority scheduling ensures that critical safety conditions are handled immediately without delay.

## 🏁 Conclusion
This project demonstrates a scalable real-time automotive safety architecture where multiple subsystems operate concurrently and a central controller ensures safe and efficient decision-making under critical conditions.

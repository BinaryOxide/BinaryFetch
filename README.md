# BinaryFetch - Ongoing Development

BinaryFetch is an evolving, lightweight system information tool inspired by Neofetch, built in C++ and optimized for Windows. This README documents the current development progress, upcoming features, and active tasks.

---

## 🚀 Overview

BinaryFetch displays essential system information such as:

- OS Details

- CPU Information

- RAM Capacity, Type & Speeds

- GPU Information (Basic + Advanced)

- Storage Information (Per-disk usage, alignment formatting)

- Custom ASCII Art Rendering (Optional)

This project is actively evolving with continuous improvements across code structure, performance, and feature coverage.

---

## 📌 Current Development Status

### ✔️ Completed / Stable Modules

- **ASCII Art Loader** (with fallback if missing)

- **Operating System Info** (OSInfo)

- **CPU Information** (CPUInfo)

- **Memory Information**
  
  - RAM capacity per slot
  
  - RAM type & speed fetching
  
  - Slot-detection

- **GPU Information**
  
  - Basic GPU detection
  
  - Detailed GPU stats (Dedicated VRAM, utilizes NVML if available)

- **Storage Information**
  
  - All disk detection
  
  - Filesystem type
  
  - Used/Free space calculation
  
  - Percentage calculation per disk
  
  - Alignment formatting (0000.00 style)

---

## 🔨 Work In Progress (Ongoing)

- **Refactoring Code Structure** into cleaner modules

- **Improved Alignment System** for Storage Summary

- **More Detailed GPU Info** (temperature, clocks, fan speeds – if libraries allow)

- **Better Error Handling** across all modules

- **Optimizing for Faster Load Time** (remove redundant calls)

- **Improved ASCII Art Handling** (auto-resize for smaller terminals)

---

## 🧪 Upcoming Features & Ideas

- **Config File (config.json)**
  
  - Theme colors
  
  - Toggle modules ON/OFF
  
  - ASCII art path selection

- **Plugin System**
  
  - Users can write small C++/DLL plugins for extra info

- **Cross-platform Support** (Linux first)

- **Fancy Colored Output** using ANSI escape sequences

- **Benchmark Mode**
  
  - CPU quick benchmark
  
  - Disk quick I/O test

---

## 🗂️ Folder Structure (Planned Final Layout)

```
BinaryFetch/
├─ src/
│  ├─ main.cpp
│  ├─ ascii_art.cpp
│  ├─ cpu_info.cpp
│  ├─ gpu_info.cpp
│  ├─ memory_info.cpp
│  ├─ storage_info.cpp
│  └─ utils.cpp
├─ include/
│  ├─ ascii_art.h
│  ├─ cpu_info.h
│  ├─ gpu_info.h
│  ├─ memory_info.h
│  ├─ storage_info.h
│  └─ utils.h
├─ assets/
│  └─ AsciiArt.txt
├─ config/
│  └─ config.json (future)
└─ README.md
```

---

## ✍️ Developer Notes

- BinaryFetch is built entirely for learning + showcasing real Windows API usage.

- Focused on low-level system programming using **Win32 API**, **WMI**, and **NVML** (optional).

- Project meant for internship portfolio & personal improvement.

---

## 🤝 Contributing

For now, BinaryFetch is a solo project. Future contributions may be accepted after version 1.0.

---

## 📜 License

To be added soon (likely MIT).

---

## 📅 Last Updated

**December 2025**
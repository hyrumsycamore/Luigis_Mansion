# Luigi's Mansion: ESP32 Embedded Edition
A hardware-synchronized, two-player embedded game built from the ground up with custom C drivers and a low-latency rendering engine.
## 📸 Photo Gallery
![IMG_2851](https://github.com/user-attachments/assets/c6ae6ac6-7ad3-487e-bdbc-93c53245f1e3)
![IMG_2850](https://github.com/user-attachments/assets/d37d4482-9a43-4ed2-9bbf-7b2d818d057e)
![IMG_2849](https://github.com/user-attachments/assets/ea990108-cd99-4ed9-af2b-f248653989fa)
![IMG_2848](https://github.com/user-attachments/assets/8a7190cb-1817-486f-860a-b36a1f930eeb)
![IMG_2852](https://github.com/user-attachments/assets/c0021eb7-d7b7-4888-8003-3bfa051d2e10)



# ------------------- Overview -------------------
This project is a recreation of Luigi’s Mansion mechanics designed to run on the ESP32 microcontroller. Unlike standard game development using high-level engines, this project involved writing the entire stack—from hardware abstraction layers (HAL) for joysticks and buttons to a custom UART communication protocol for real-time multiplayer synchronization.

# ------------------- Technical Highlights -------------------
Microcontroller: ESP32 (Dual-Core)

Language: C / Arduino Framework

Peripherals: Analog Joysticks, Digital Buttons, 320x240 SPI Display

Communication: Custom UART-based state synchronization protocol

Architecture: Modular driver-based design with an integrated gameplay loop

# ------------------- Engineering Features -------------------
**Low-Level Driver Development**
To minimize latency and maximize control, I engineered custom peripheral drivers from scratch:

Analog Input: Implemented smoothing and dead-zone logic for dual-axis joysticks to ensure precise character movement.

Audio Engine: Developed a non-blocking sound driver to handle game SFX without interrupting the main gameplay loop.

Display Logic: Programmed a custom rendering engine to manage sprites, collision detection, and UI updates at 30+ FPS.

### 2. Custom UART Protocol & Bit Packing
To minimize overhead, I avoided standard serialization libraries (like JSON) in favor of raw bit manipulation.
* **Method:** Bit Packing.
* **Implementation:** Two 32-bit spatial coordinates are compressed into a single 64-bit payload using bitwise shifting. This ensures atomic updates of player position.
* **The Logic:**
    ```c
    // Packing Logic (Sender)
    uint64_t packed = ((uint64_t)x_pos << 32) | y_pos;
    
    // Unpacking Logic (Receiver)
    x_pos = (coord_t)(packed >> 32);
    y_pos = (coord_t)(packed & 0xFFFFFFFF);
    ```
* **Non-Blocking I/O:** The UART driver is configured with a 0ms timeout (`uart_read_bytes(..., 0)`), ensuring the game loop never hangs waiting for data packets.

### 3. Resource Management
* **Interrupts:** Used GPIO interrupts for tactile button feedback to ensure zero missed inputs during high-load rendering cycles.
* **Memory:** Optimized sprite storage using Struct packing to fit within the ESP32's DRAM limits.
Hardware Timers: For precise audio playback and physics calculations.
## Project Structure  
```text  
├── main  
│   ├── com/            # Custom UART communication protocols  
│   ├── config/         # Global game constants & macros  
│   ├── flashlight/     # State machines for light cone & battery logic  
│   ├── game/           # Main game tick & state coordinator  
│   ├── graphic/        # Bitmap assets (Walls, Player, UI) & Render logic  
│   ├── player/         # Physics integration & player structs  
│   └── audio/          # Raw audio arrays (formerly listed as mixkit files)  
└── README.md

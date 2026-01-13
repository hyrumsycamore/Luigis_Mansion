# Luigi's Mansion: ESP32 Embedded Edition
A hardware-synchronized, two-player embedded game built from the ground up with custom C drivers and a low-latency rendering engine.
## 📸 Photo Gallery

### Hardware Setup
<p align="center">
  <img src="https://github.com/user-attachments/assets/0d69b879-40d3-4945-a462-7d7cac024784" width="30%" />
  <img src="https://github.com/user-attachments/assets/8e1d739a-e0bc-41c5-b88d-5f8a94a1dd98" width="30%" />
  <img src="https://github.com/user-attachments/assets/93f50195-4ad5-4a23-a490-a566796537ea" width="30%" />
</p>
<p align="center">
    <em>Left to Right: Wiring harness close-up, full breadboard prototype, unit powered on displaying the map.</em>
</p>

<br>

### Gameplay Close-ups
<p align="center">
  <img src="https://github.com/user-attachments/assets/7648e6ca-5d88-4e10-8399-30fb0732775f" width="45%" />
  <img src="https://github.com/user-attachments/assets/18709e2c-7d38-4409-a5c3-fcbefde973dc" width="45%" />
</p>
<p align="center">
    <em>Close-ups of the custom rendering engine showing player sprites and wall collision assets.</em>
</p>

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

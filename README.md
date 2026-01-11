# Luigi's Mansion: ESP32 Embedded Edition
A hardware-synchronized, two-player embedded game built from the ground up with custom C drivers and a low-latency rendering engine.

# ------------------- Overview -------------------
This project is a recreation of Luigi’s Mansion mechanics designed to run on the ESP32 microcontroller. Unlike standard game development using high-level engines, this project involved writing the entire stack—from hardware abstraction layers (HAL) for joysticks and buttons to a custom UART communication protocol for real-time multiplayer synchronization.

# ------------------- Technical Highlights -------------------
Microcontroller: ESP32 (Dual-Core)

Language: C / Arduino Framework

Peripherals: Analog Joysticks, Digital Buttons

Communication: Custom UART-based state synchronization protocol

Architecture: Modular driver-based design with an integrated gameplay loop

# ------------------- Engineering Features -------------------
**Low-Level Driver Development**
To minimize latency and maximize control, I engineered custom peripheral drivers from scratch:

Analog Input: Implemented smoothing and dead-zone logic for dual-axis joysticks to ensure precise character movement.

Audio Engine: Developed a non-blocking sound driver to handle game SFX without interrupting the main gameplay loop.

Display Logic: Programmed a custom rendering engine to manage sprites, collision detection, and UI updates at 30+ FPS.

**Multi-Unit Synchronization (Networking)**
Designed a proprietary UART-based protocol to connect two independent hardware units.

Packet Structure: Implemented a lightweight data packet system to transmit player coordinates, orientation, and "flashlight" status.

**Resource Management**
The game was optimized for the ESP32’s architecture, utilizing:

Interrupts: For tactile button feedback to ensure zero missed inputs.

Hardware Timers: For precise audio playback and physics calculations.
# ------------------- Included Files -------------------
**main/com**  
  Handles UART Communications

**main/config**  
  Holds the majority of constants

**main/flashlight**  
  The state machines for the flashlight and batter

**main/player**  
  The state machine for the player

**main/game**  
  The state machine for the game, controls the main game tick

**main/graphic**  
  The graphics for the game

**main/mixkit_arcade_retro**  
  Sound Effect

**main/short-beep-tone-47916**  
  Sound Effect

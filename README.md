# Nyx – Space Simulation Engine

**Nyx** is a real-time 3D simulation engine built in C++ that can visually represents any stellar system with physically plausible motion, lighting, and atmospheric effects. Designed with modularity and extensibility in mind, Nyx offers an interactive editor for exploring planetary systems, tweaking parameters, and simulating orbital dynamics.

---

## Features

- **Physically-Based Planetary Rendering**  
  Realistic planet textures, lighting, and shading with atmospheric scattering simulation.

- **Orbital Mechanics Simulation**  
  Each celestial body has mass, velocity, angular velocity, and acceleration parameters, simulating real-time dynamics.

- **Interactive Editor**  
  Built-in editor allows you to:
  - View & modify transform, rotation, and scale
  - Inspect and tweak rigidbody physics properties
  - Adjust atmosphere scattering for visual tuning
  - Organize objects via hierarchy

- **Content Browser**  
  Easily navigate and manage assets, textures, and external modules.

- **CMake Build System**  
  Portable and cross-platform project setup with `CMakeLists.txt` and `CMakePresets.json`.

---

## Built With

- **C++17**
- **OpenGL** (rendering backend)
- **QT** (for editor UI)
- **Custom ECS-style architecture** (Entity-Component-System)
- **CMake** for build configuration and dependency management

---

## Preview

### Earth and Moon – Atmosphere and Scattering
<img width="1656" height="1000" alt="Screenshot 2025-11-06 174730" src="https://github.com/user-attachments/assets/3b69eab1-999e-4cc1-a7e6-d4371d18d1c9" />

### Mars with Grid Overlay – Dynamic Lighting
<img width="1598" height="1013" alt="Screenshot 2025-11-06 175039" src="https://github.com/user-attachments/assets/de81a063-a13d-483a-82dd-3c16c7e1c2d3" />

### Full Editor View – Scene Hierarchy, Inspector, and Content Browser
<img width="2000" height="925" alt="Screenshot 2025-11-06 174846" src="https://github.com/user-attachments/assets/24777121-92b4-4b18-b4e2-ad4f5286b2fa" />

> *Note: Images above are real screenshots taken from the interactive Nyx Editor.*

---

## Getting Started

1. **Clone the repo**
   ```bash
   git clone https://github.com/Alpercomen/Nyx.git
   cd Nyx

2. **Generate project files using CMake**
   ```bash
   cmake -S . -B Build
   cmake --build Build

3. **Run the Editor**
   - Navigate to the build directory and launch the Nyx Editor executable.

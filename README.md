# Freya

Freya is a cross-platform paint application built using **C++**, **SFML 3.0**, and **ImGui**. It is designed with a modular architecture allowing for scalable, clean, and reusable code. The project uses [Premake](https://github.com/premake/premake-core) for build configuration and project file generation.

## Features

- 🖌️ Drawing and erasing with adjustable brush size, spacing, and opacity  
- 🧾 Fully localized UI with Turkish and English support  
- 📁 Project saving/loading to a custom `.freya` binary format  
- 🧠 Undo support  
- 🔁 Canvas transformations (rotate, flip, resize)  
- 📤 Image export (PNG, JPG, BMP)  
- 🧰 Modular UI panels and tool system (brush, eraser, toolbar, color picker)  
- 🗂️ Future-proof architecture for upcoming layer support  

---

## Project Structure
 
- `Freya-App/Source` – Freya-specific logic and UI, application loop, ImGui integration, Project-Settings-Localization Managers  
- `Vendor/` – SFML, ImGui-SFML, Native File Dialog, and Premake binaries  
- `Scripts/` – Build setup scripts for Windows and Linux  

---

## Getting Started

1. Clone the repository

```bash
git clone https://github.com/Katlicia/Freya.git
cd Freya
```

2. Run the Setup Script
Open the `Scripts/` directory and run the appropriate `Setup` script to generate projects files. You can edit the setup scripts to change the type of project that is generated - out of the box they are set to Visual Studio 2022 for Windows and gmake2 for Linux.

3. Building
On Windows: Open Freya.sln in Visual Studio and build the App project.
On Linux: Run make in the root directory after generating files with gmake2.

4. Entry point is under the App/Source/App.cpp if you run the project you should see the SFML Works! screen.

## Notes
The default save path for projects is under the user's Documents/Freya Projects directory.
Projects are stored as .freya binary files containing canvas state and pixel data.
Exported images are saved independently of project files.

## Roadmap
- ✅ Custom binary project format
- ✅ Undo/Redo system
- ✅ Canvas transformations
- 🧪 More brush support (planned)
- 🧪 Layer support (in the future)
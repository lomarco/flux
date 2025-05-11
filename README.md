<h1 align="center">Flux</h1>
<p align="center"><em>Is a lightweight, open-source GNU/Linux command shell</em></p>
<p align="center">
  🐞 <a href="https://github.com/VorPijakov23/flux/issues/new?template=bug_report.md">Report a Bug</a> ·
  ✨ <a href="https://github.com/VorPijakov23/flux/issues/new?template=feature_request.md">Request a Feature</a> ·
  💬 <a href="https://github.com/VorPijakov23/flux/discussions">Ask a Question</a>
</p>

<br />

<details open="open">
<summary>Table of Contents</summary>

- [About](#about)
- [Why Flux?](#why_flux)
- [Features](#features)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
  - [Build flux](#build-flux)
    - [Build with LLVM](#built-with-llvm)
  - [Install](#install)
    - [Quick install](#quick-install)
  - [Launch Flux](#launch-flux)
- [Development](#development)
- [License](#license)
- [Author & Copyright](#author_copyright)
</details>

---

## About
<table>
<tr>
<td>

Flux was born out of a desire to create a command shell that strips away unnecessary complexity without sacrificing speed or reliability.

As an open-source, lightweight GNU/Linux shell, Flux focuses on delivering a **clean**, efficient terminal experience-fast to start, easy to use, and **free from the bloat** that slows down everyday workflows.
</td>
</tr>
</table>

## Why Flux?

- **Faster than Bash and Zsh:** Flux starts up in milliseconds, making it ideal for scripting, embedded systems, and power users who value speed.
- **Minimalism without compromise:** No unnecessary features, only what you need for efficient daily work.
- **Easy to build and hack:** Clean codebase, minimal dependencies, and clear documentation make it perfect for contributors and system integrators.
- **Ideal for old hardware:** Flux consumes less RAM and CPU, breathing new life into legacy systems.
- **Open-source and community-driven:** Transparent development, MIT license, and open to feature requests.

> **Tired of slow shell startups and bloated configs? Flux is your solution.**

## Features

- 🚀 **Instant startup** - launches in milliseconds
- 🧩 **Minimal dependencies** - easy to build and install
- ⚡ **Low memory footprint** - ideal for embedded and legacy systems
- 🔒 **Open-source (MIT License)**
---

<!-- Screenshots block and dont forget "---" in end -->

## Requirements

- GNU/Linux
- GCC or Clang/LLVM (for recommended build)
- `make` utility

---

## Getting Started

### Build Flux
```bash
make
```
*Builds Flux with default settings.*

#### Build with LLVM (Recommended)
```bash
make LLVM=1
```
*Builds Flux using LLVM for better performance.*

### Install
```bash
sudo make install
```
*Installs Flux system-wide.*

#### Quick install
```bash
make LLVM=1 && sudo make install && make clean
```

### Launch Flux
```bash
flux
```
---

## 👩‍💻 Development
See the [Style Guide](STYLEGUIDE.md) for internal development standards.

---

## 📄 License
Flux is licensed under the MIT. See the [here](LICENSE) for details.

---

## 👤 Author & Copyright
© 2025 [VorPijakov23](https://github.com/VorPijakov23). All rights reserved.

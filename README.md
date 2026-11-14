SpongeBob SquarePants: Battle for Bikini Bottom (GameCube)  
[![Build Status]][actions] <!--[![Code Progress]][progress] [![Data Progress]][progress]--> [![Discord Badge]][discord]
=============

[Build Status]: https://github.com/seilc/bfbb/actions/workflows/build.yml/badge.svg
[actions]: https://github.com/seilc/bfbb/actions/workflows/build.yml
<!--
decomp.dev progress badges (hidden until the project is registered on decomp.dev)
See https://decomp.dev/api for an API overview.
-->
[Code Progress]: https://decomp.dev/seilc/bfbb.svg?mode=shield&measure=code&label=Code
[Data Progress]: https://decomp.dev/seilc/bfbb.svg?mode=shield&measure=data&label=Data
[progress]: https://decomp.dev/seilc/bfbb
[Discord Badge]: https://img.shields.io/discord/829152115322257436?color=%237289DA&logo=discord&logoColor=%23FFFFFF
[discord]: https://discord.gg/dVbGFdYU6A

A work-in-progress matching decompilation of *SpongeBob SquarePants: Battle for Bikini Bottom* for the GameCube (USA,
`GQPE78`), built on Heavy Iron's EvilEngine. Much of the engine is shared with the
[Ratatouille prototype decomp](https://github.com/seilc/rat), [Rise of the Underminer](https://github.com/seilc/rotu),
[The Incredibles](https://github.com/seilc/incredibles) and
[The SpongeBob SquarePants Movie](https://github.com/seilc/tssm), which this project ports matched code from.

Based on the community decompilation [bfbbdecomp/bfbb](https://github.com/bfbbdecomp/bfbb) at
[`d8e8eac3`](https://github.com/bfbbdecomp/bfbb/commit/d8e8eac3), whose history this repository keeps; thanks to
its contributors. Upstream changes after that commit are not merged.

This repository does **not** contain any game assets or assembly whatsoever. An existing copy of the game is required.

Full disclosure: work in this repository after `d8e8eac3` is AI-assisted. New code is decompiled with the help of an
LLM, then reviewed and checked against the original binary. Matching code is byte-identical to the original, but
names, types and source structure beyond what the binary and the PS2 build's debug info prove may be guesses. The code
inherited from bfbbdecomp/bfbb was written by its contributors.

It builds the following DOL:

main.dol: `sha1: 306526d90b48e99894c3138f5fc8f2716d9fecf6`

Supported versions:

- `GQPE78` - Battle for Bikini Bottom (USA), retail `main.dol`

The game code builds with `GC/2.0p3`, our model of the CodeWarrior version it needs (not in decomp.dev's compiler
set), built from [our fork](https://github.com/seilc/mweppc) of the [doldecomp/mweppc](https://github.com/doldecomp/mweppc)
decompilation of the compiler. The build downloads it with the other tools (`configure.py --compilers DIR` skips the
download, so DIR must then hold `GC/2.0p3`). decomp.me has no 2.0p3, so a scratch there may not match the build.

# Dependencies

## Windows

On Windows, it's **highly recommended** to use native tooling. WSL or msys2 are **not** required.  
When running under WSL, [objdiff](#diffing) is unable to get filesystem notifications for automatic rebuilds.

- Install [Python](https://www.python.org/downloads/) and add it to `%PATH%`.
  - Also available from the [Windows Store](https://apps.microsoft.com/store/detail/python-311/9NRWMJP3717K).
- Download [ninja](https://github.com/ninja-build/ninja/releases) and add it to `%PATH%`.
  - Quick install via pip: `pip install ninja`

## macOS

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages):

  ```sh
  brew install ninja
  ```

- Install [wine-crossover](https://github.com/Gcenx/homebrew-wine):

  ```sh
  brew install --cask --no-quarantine gcenx/wine/wine-crossover
  ```

After OS upgrades, if macOS complains about `Wine Crossover.app` being unverified, you can unquarantine it using:

```sh
sudo xattr -rd com.apple.quarantine '/Applications/Wine Crossover.app'
```

## Linux

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).
- For non-x86(\_64) platforms: Install wine from your package manager.
  - For x86(\_64), [wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

# Building

- Clone the repository:

  ```sh
  git clone https://github.com/seilc/bfbb.git
  ```

- Using [Dolphin Emulator](https://dolphin-emu.org/), extract your game to `orig/GQPE78`.
  ![](assets/dolphin-extract.png)
  - To save space, the only necessary files are the following. Any others can be deleted.
    - `sys/main.dol`
- Configure:

  ```sh
  python configure.py
  ```

  To use a version other than `GQPE78` (NTSC-U), specify it with `--version`.

- Build:

  ```sh
  ninja
  ```

# Diffing

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

Select an object from the left sidebar to begin diffing. Changes to the project will rebuild automatically: changes to source files, headers, `configure.py`, `splits.txt` or `symbols.txt`.

![](assets/objdiff.png)

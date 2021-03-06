# Bunnymod XT

### VAC BAN WARNING: Do NOT connect to servers with this injected, or you might get VAC banned!

Bunnymod XT (BXT for short) is a cross-platform tool that provides speedrunning and TAS-related features for GoldSource games such as Half-Life and its modifications.

Used by hundreds if not thousands of speedrunners worldwide.

Bunnymod XT provides state-of-the-art speedrunning and TASing features and utilities, ranging from detailed HUDs to advanced autostrafing. It is a successor to the obsoleted hlspbunny and Bunnymod Pro.

## Why?

Despite the "mod" in Bunnymod XT, this is not a mod in the usual sense. Most Half-Life modifications work by modifying the Half-Life SDK and distributing the resulting DLLs. This includes the deprecated Bunnymod Pro. The downside is that while you can "mod" Half-Life itself, you cannot "mod" another Half-Life mod. This is a serious limitation for speedrunners intending to speedrun Half-Life mods and expansions.

To rectify this, Bunnymod XT injects into the Half-Life process while leaving every file on the disk intact. This means all modifications are done on-the-fly in RAM. This also means Bunnymod XT supports a wider range of Half-Life engines, from WON to the latest Steam.

## Installation

### Windows

The instructions are described [here](https://github.com/YaLTeR/BunnymodXT/wiki).

### Linux

1. Download or build **libBunnymodXT.so**.
2. Download [this helper script](https://gist.github.com/Matherunner/18b838236670df58ddc989d42a594eec).
3. Run `chmod +x runhl.sh` to make the script executable.
4. Edit the helper script by appending the absolute path of **libBunnymodXT.so** to the end of the `export LD_PRELOAD=` line. For instance, `export LD_PRELOAD=/home/me/dir/libBunnymodXT.so`.
5. Launch Half-Life from the terminal by executing `./runhl.sh`.

## Documentation

[On the Wiki pages](https://github.com/YaLTeR/BunnymodXT/wiki).

### Environment variables

- **BXT_SCRIPT** - if set to a filename of a hltas script, loads the non-shared RNG from that script on load.
- **BXT_LOGFILE** - if set, logs all Bunnymod XT messages into a file with that filename.
- **SPTLIB_DEBUG** - if set to 1, logs all dlopen, dlclose and dlsym calls.

## Building

### Windows

Building on Windows requires

- Visual Studio 2017 or 2019
- [Boost](http://www.boost.org/)
- [RapidJSON v1.1.0](https://github.com/miloyip/rapidjson/releases/tag/v1.1.0)
- [Rust](https://rustup.rs/)
  - The `i686-pc-windows-msvc` target must be installed. You can do that using `rustup target add i686-pc-windows-msvc`.

Run the following commands, replacing `path\to\boost\base\dir` and `\path\to\rapidjson\base\dir` with paths to Boost and RapidJSON base directories respectively:

    git clone https://github.com/YaLTeR/BunnymodXT
    cd BunnymodXT
    git submodule update --init --recursive
    mkdir build
    cd build
    cmake -A Win32 -DBOOST_ROOT=path\to\boost\base\dir -DRapidJSON_ROOT=path\to\rapidjson\base\dir -Wno-dev ..

Then compile the `ALL_BUILD` project from the generated VS solution.

If you want to make a release build, you need to specify `-DCMAKE_BUILD_TYPE=Release` in the `cmake` command line arguments. This is needed because the Rust CMake module uses that variable to determine whether to build the crate in release or debug mode.

### Linux

Building on Linux requires

- A recent GCC or Clang toolchain
- Boost
- [RapidJSON v1.1.0](https://github.com/miloyip/rapidjson/releases/tag/v1.1.0)
- Rust: either from your distribution's packages, or from [rustup](https://rustup.rs/).
  - The `i686-unknown-linux-gnu` target must be installed. You can do that using `rustup target add i686-unknown-linux-gnu`.

Many of these dependencies can be installed from a package manager.

    git clone https://github.com/YaLTeR/BunnymodXT
    cd BunnymodXT
    git submodule update --init --recursive
    mkdir build
    cd build
    cmake -Wno-dev ..
    make

Note that `-DBOOST_ROOT` and `-DRapidJSON_ROOT` are not required as CMake should be able to find the location of these libraries in your system. In case it couldn't, you need to specify them manually like the case on Windows.

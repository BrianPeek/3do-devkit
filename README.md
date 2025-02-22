This is a 3DO Opera development environment for Linux and Window. It
is built from a number of components both new and old. The hope is
that by making setup more turnkey and bringing documentation and
examples into one location it will help facilitate more homebrew on
the 3DO.


## Components

### Compilers

* Norcroft ARM C v4.91 (ARM Ltd SDT2.51) [Build number 130] for Linux
* Norcroft ARM C v4.91 (ARM Ltd SDT2.51) [Build number 128] for Windows
* Norcroft ARM C++ v0.61/v4.91 (ARM Ltd C++1.11) [Build number 130] for Linux
* Norcroft ARM C++ v0.61/v4.91 (ARM Ltd C++1.11) [Build number 128] for Windows


### Libraries

* Original 3DO libraries from Portfolio 2.5 SDK
* [cpplib](https://github.com/trapexit/3do-cpplib): basic replacement
  C++ standard library
* [svc_funcs](https://github.com/trapexit/3do-svc-funcs): provides
  access to kernel functions while in supervisor mode
* [svc_mem](https://github.com/trapexit/3do-svc-mem-device): example
  device driver and its library
* [example_folio](https://github.com/trapexit/3do-example-folio):
  example Folio (shared library)
* ttl: trapexit's template library (simplistic replacement to STL)
* Roguewave STL which originally came with ARM C++ (probably not worth
  using)


### Documentation

It is *strongly* suggested that new developers start with the original
3DO SDK documentation. While the layout is imperfect the documentation
is reasonably thorough. The 3DO uses a high level operating system
that abstracts the hardware and provides many features such as
semaphores, threading, message passing, signals, etc. An initial focus
on the [OS
APIs](https://3dodev.com/documentation/development/opera/pf25/ppgfldr/pgsfldr/00pgs)
and how
[graphics](https://3dodev.com/documentation/development/opera/pf25/ppgfldr/ggsfldr/00ggs)
work on the console is suggested.

* [3DO SDK's "Developer's Documentation Set"](https://3dodev.com/documentation/development_documents)
  * Also found in the repo in the original form: [docs/3dosdk](docs/3dosdk)
* [ARM60 datasheet](docs/cpu)
* [ARM SDT and ARM C++ docs](docs/compilers)
* [Development notes](docs/devnotes/README.md): Collection of notes
  collected from the homebrew community.

More can be found at https://3dodev.com


### Tooling

* [3it](https://github.com/trapexit/3it/releases): trapexit's 3DO
  Image Tool
* [3at](https://github.com/trapexit/3at/releases): trapexit's 3DO
  Audio Tool
* [3dt](https://github.com/trapexit/3dt/releases): trapexit's 3DO Disc
  Tool
* [3ct](https://github.com/trapexit/3ct/releases): trapexit's 3DO
  Compression Tool
* [modbin](https://github.com/trapexit/modbin/releases): trapexit's
  recreation of the original SDK's tool by the same name
* 3doiso v0.1 by nikk
* 3DOEncrypt v0.6a by Charles Doty
* MakeBanner v1.0b by Charles Doty
* BMPTo3DOCel v0.6a by Charles Doty
* BMPTo3DOImage v1.0b by Charles Doty
* BMPTo3DOAnim v0.6a by Charles Doty
* [GNU make for Windows](https://gnuwin32.sourceforge.net/packages/make.htm)

trapexit's tooling may not always be fully up to date in this repo so
double check by visiting the links provided above.


### Examples

* [README](examples/README) for more information.
* [Community Examples](examples/community): A collection of examples
  from the 3DO homebrew community.
* [Reworked Examples](examples/reworked): A collection of examples reworked from
  the original 3DO SDKs to work out of the box with this 3DO DevKit.
* [Original Examples](examples/original): These are all the examples
  from the original 3DO Portfolio and Toolkit releases in their
  original form (outside text encoding conversion.) They are not
  buildable without some effort.
* [3D 3DO Logo](src/3d_3do_logo.c)
* [CEL rotation and zoom demo](src/cel_rotation.cpp)


### Misc

* 3DO "takeme" CDROM base files from Portfolio 2.5 w/ a swapped out
  boot_code from Game Guru.


## Install

There really isn't a need to "install". You can download the repo and
start using it in place. See [#Usage](#Usage) below. However, you can
set things up in a manner similar to a global install.


### Windows

* [Download](https://github.com/trapexit/3do-devkit/archive/refs/heads/master.zip)
the dev kit.
* Uncompress and move the folder into its final location.
* Run `bin\buildtools\win\setup-3do-devkit-env.bat`
* This will setup the appropriate environment variables at a global
level. This removes the need to use `activate-env.bat` or
`activate-env.ps1` each terminal session.

Note: the devkit includes `make.bat` and `make-run.bat` which need no
activation and can be run from Explorer.


### Linux

* Download: `git clone --depth=1 https://github.com/trapexit/3do-devkit`
* Add `source /path/to/3do-devkit/activate-env` to your shell config file.


## Usage

This setup is intended to be as simple as possible to start using. To
that end it is primarily designed to be used in-place. There is no
install required. Just download, activate environment, and build.

### General

* Modify `Makefile` to change the project `NAME`. Not much else should
  need to be modified in the `Makefile` for basic usage.
* The makefile handles assembly source files (`*.s`), C files (`*.c`),
  and C++ files (`*.cpp`) in the root of `src/`.
* Add and/or remove files from `src/` as needed for your project.
* Run `make` to build object files, link executable, build ISO, and
  sign ISO for retail system usage.

See below for OS specific workflows.


### Windows

#### General

* [Download](https://github.com/trapexit/3do-devkit/archive/refs/heads/master.zip)
* Uncompress and move the folder into its final location
* From a terminal (cmd.exe or PowerShell):
  * Enter the directory: `cd 3do-devkit`
  * Source the environment: `activate-env` (CMD) or `.\activate-env` (PowerShell)
  * Run make: `make`
    * Generates `iso\helloworld.iso`
  * Run in RetroArch Opera emulator (if installed): `make run`
* From Explorer:
  * Enter the directory
  * Run `make.bat` to build the project
  * Run `make-run.bat` to run the created iso via Opera if RetroArch
    is installed
  * Run `make-clean.bat` to remove build files


#### WSL2

Same as Linux


### Linux

The ARM compile suite binaries are x86 32bit. Be sure to install 32bit
support for your distro. On Debian / Ubuntu based distros that would
be:

```console
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libc6:i386
```

Also ensure you have WINE installed. Some tools are currently only
available on Windows.


#### makefile

* Download: `git clone --depth=1 https://github.com/trapexit/3do-devkit`
* Enter the directory: `cd 3do-devkit`
* Source the environment: `source activate-env`
* Run make: `make`
  * Generates `iso/helloworld.iso`
  * Run in RetroArch Opera emulator (if installed): `make run`
* Run `make clean` to clean up build files


## Bootstrapping a New Project

If you have "installed" the dev kit as described above or activated
the environment you can easily copy code, assets, and build tools into
their own directories which will use the binaries found in the
"install" location.

To simplify the creation of these there is the `bootstrap-3do-project`
script.


### Windows

* Run `setup-3do-devkit-env.bat` (if not done already) to setup paths
  globally or `activate-env`
* Make a directory for your project
* Change the directory to your new project path
* Run `bootstrap-3do-project.bat`
* Or copy `bootstrap-3do-project.bat` to that directory and run it
  (from shell or Explorer)

This will copy all relevant files into the path and can be used the
same as described above. Feel free to remove
`bootstrap-3do-project.bat` afterwards.


### Linux

* Source `activate-env` as described above
* Make a directory for your project
* Run `bootstrap-3do-project` from within that directory
* You can also run `bootstrap-3do-project PATH`


## RetroArch Opera Setup

`make run` will launch the Opera core of RetroArch if installed and
run the built ISO. RetroArch can be found at https://retroarch.com but
you will need ROMs for Opera to work correctly.

The script `download-retroarch-opera-roms` will attempt to download
required ROMs to the "system" directory of RetroArch if already
installed. On Windows or Linux launch from a file explorer or a
terminal.

NOTE: The scripts look in known directories to copy the ROMs to but it
is possible you have a different setup. You can just go to the
[site](https://3dodev.com/software/roms) and download the ROMs
manually and place them into the RetroArch `system` folder.

* https://3dodev.com/_media/roms/panafz1.bin
* https://3dodev.com/_media/roms/panafz1j-kanji.bin


## DevKit Layout

* bin/: All core binaries such as compilers, linkers, media
  conversion tools, and misc tooling.
* src/: Directory storing all application source.
* takeme/: Directory storing all CDROM artifacts. Target for final
  "Launchme" executable. The name "takeme" originates from the
  original 3DO SDK.
* art/: 3DO artwork. Currently only original 3DO SDK art.
* docs/: Misc documentation from the original SDK and compiler suites.
* include/: All include files from original SDK and community projects.
* lib/: All libraries from original SDK and community projects.
* examples/: Examples from all available original 3DO SDK releases as
  well as new and reworked examples.
* build/: Automatically created directory during build to store object
  files.
* iso/: Automatically created directory during build to store ISO file.


## Media Conversion

### images

[3it](https://github.com/trapexit/3it) is a comprehensive tool
supporting conversion to and from CELs, IMAGs, Banners, etc. Older
tools are included for completeness but should not be needed.

Read more about CEL formats at:
* https://3dodev.com/documentation/file_formats/media/image/cel
* https://3dodev.com/documentation/file_formats/media/container/3do


### audio

The 3DO can [handle multiple sample rates, sample sizes, channels, and
codecs.](https://3dodev.com/documentation/development/opera/pf25/ppgfldr/mgsfldr/mpgfldr/03mpg004)

SDX2 compresses the audio to 8bits per sample and according to the
original author, Phil Burk, sounds noticeably better than using raw
8bit samples. Intel/DVI ADP4 compresses down to 4 bits per sample but
only allows for mono audio and is notably worse sounding compared to
SDX2. SDX2 will typically be best for music and ADP4 for sound effects.

[ffmpeg](https://ffmpeg.org) can be used to convert files to a couple
raw audio formats which 3DO can use however ffmpeg does not currently
have a SDX2 encoder (only decoder) or a Intel/DVI ADP4 encoder or
decoder (though adpcm_ima_ws is similar.)

[trapexit's 3at](https://github.com/trapexit/3at) can be used to
convert to and from SDX2 and Intel/DVI ADP4.


### video

A Cinepak library was included in the original SDK. Unfortunately,
`ffmpeg` does not support the 3DO Stream container format nor does the
Cinepak encoder generate frames which align properly for the 3DO.

Till a new decoder is written or `ffmpeg` modified to provide proper
alignment you will need to use original Classic MacOS software.

* https://3dodev.com/tutorials/trapexit/creating_3do_compatible_fmv
* http://3dodev.com/software/sdks#prebuilt_qemu_macos_9_vm


## TODO

* Project files for popular IDEs.
* Rework libc. Add missing functions.
* Rebuild original privileged libraries and enhance them where possible.
* Continue to enhance the C++ standard library replacement.
* C++ based 3DO specific libraries.
* Create a new iso building tool.
* Create a new iso encrypt tool.
* More examples.
* Better compiler support. Possibly use CLANG or GCC to generate assembly and
  translate it to work with Norcroft armasm.
* More languages? If you have a ARMv3 compatible compiler and AIF linker
  (even if just different versions of Norcroft compilers) please reach out.
* A version of the Opera emulator tailored for development work?


## FAQ

### Why not use a more modern compiler suite?

The 3DO is an early ARMv3 based system which used ARM's Norcroft
compiler, relocatable AIF executable format, AOF object format,
etc. Support for these formats were abandoned by ARM around the time
the console was retired and the limited open source compiler suite
support for these formats were removed long long ago. Support for
ARMv3 era CPUs have also been removed.

trapexit has spoken with John Fitch of Codemist regarding licensing or
open sourcing their ARM compiler suite as well as individuals who
maintain the compiler suite for RISCOS. Unfortunately, neither were
able to help. The latter having narrowly scoped licensing with no
ability to sublicense.

Adding ARMv3 support to a modern compiler might not be so hard given
ARMv4 is still supported by but adding AIF and AOF to modern linkers
would likely be a decent amount of work and maintenance. There are
other ideas ranging from an ELF to relocatable AIF converter to
transpiling ARMv4 assembly to `armasm` compatible assembly.


## Thanks

* @ArmSoftwareDev on Twitter and Arm Support: for providing me with
copies of ARM SDT 2.51 and ARM C++ 1.11. After reaching out to former
Norcroft employees, software archivists, and even Bjarne Stroustrup
without success finding a copy of ARM C++ from the 3DO era I reached
out to ARM directly and they were able to find some copies and offered
them to me for this project.
* Everyone at The 3DO Community Discord.
* XProger: author of 3DO OpenLara and Wipeout prototype ports.
* Shaun 3DOHD: author of REAL 3DO Tetris and MK2 3DO Edition.
* zyzix: author of Biofury, JinglesDefense, REAL Tempest, and the Matinicus engine.


## Links

* 3DO Development Repo: https://3dodev.com
* 3do-devkit: https://github.com/trapexit/3do-devkit
* Portfolio OS: https://github.com/trapexit/portfolio_os
* The 3DO Community Discord: https://discord.com/invite/kvM9cQG


## Donations / Sponsorship

If you find 3do-devkit and the surrounding tooling useful please
consider supporting its ongoing development.

https://github.com/trapexit/support

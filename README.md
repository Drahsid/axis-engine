# Axis Engine

### What is this?
This is a project with the goal of being a cross-platform game engine that supports the N64, Windows, and Debian based Linux distributions. For the N64, this uses the N64 SDK without nusys.

### What is the current progress
Not far. Not ready for personal projects.

### What are the goals
The end goal is to have a reasonably "easy to use" cross-platform game engine and toolset for producing games that are cross-platform with the N64.

### Mini Projects in this repository
#### [axis](./axis)
This is the core project. Currently this is a clean reproduction of the onetri demo. To build this, install the n64 sdk and run `make`.

#### [fspack](./tools/fspack)
fspack, or filesystem-pack is a tool that builds a filesystem for packing into a rom.
To build this, just run `gcc main.c`

#### [onetri-OpenGL](./onetri-OpenGL)
An OpenGL reproduction of onetri. This project is being used for testing.


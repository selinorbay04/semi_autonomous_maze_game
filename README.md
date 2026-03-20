# Semi-autonomous Maze Game

This repo is based on starter code for Northwestern course CE346 based on Micro:bit v2.

This repository has applications and board initialization that connects to the
build system provided by [nrf52x-base](https://github.com/lab11/nrf52x-base).

## Overview

The purpose of this repo is to create a driving line following robot (powered by a Micro:bit) which can autonomously drive along a line until it reaches an intersection. Upon reaching an intersection it sends a request to a secondary Micro:bit, prompting a user to choose to go left or right. It waits in place until it receives a command.

After recieving a command, the robot takes the path it was given until it reaches another intersection, a dead-end, or the end of the maze (denoted with a red marker). If it reaches:

    1) Another Intersection: it repeats the steps described above.
    2) A Dead-End: it turns around by itself and repeats the steps described above.
    3) The End: it turns around and attempts to trace its way back to the start of the maze, optimizing the path the user initially took.

During the initial traversal of the maze, the robot keeps track of all decisions using a stack (left, right, and backtrack). When it finishes, it looks back over the stack and substitutes all incorrect paths (of any depth) with the proper turn that should have been taken. Then it makes its way back by popping from the stack and choosing the opposite decision until the stack is empty.

## Application Files

The application files (directories in which to run `make flash`) are located in `./software/apps/drive/` and `./software/apps/radio_recv/`. Helper functions and drivers are location in `./software/apps/include/`.

The `drive` directory contains code for the robot, and `radio_recv` contains code for the controller. All other app directories should be ignored, they were just scratch work.

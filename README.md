# Platformer Sandbox

A simple sandbox project in C to play around with some platformer concepts. Uses [Raylib](https://github.com/raysan5/raylib) for graphics stack.

## Building

Raylib requires some Xorg libraries (see the [`flake.nix`](./flake.nix) file for a comprehensive list) to be installed system-wide.

```sh
make # outputs a `game` binary into `./build/`
```

## Running

```sh
make && build/game
# OR
make run
```

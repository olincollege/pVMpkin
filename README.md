# pVMpkin -- LC-3 Virtual Machine

Hi! We implemented our own LC-3 Virtual Machine (VM) in C following this [tutorial](https://www.jmeiners.com/lc3-vm/#what-is-a-virtual-machine-) by Justin Meiners and Ryan Peddleton ([GitHub Repo](https://github.com/justinmeiners/lc3-vm)). Our learning goals with this project included:

1. Building a conceptual understanding of virtualization
2. Exploring each of the components of a VM
3. Implementing our own version of a VM

## Setup

If you're on a Linux machine, first clone this repository with

```
git clone git@github.com:olincollege/pVMpkin.git
```

and download SDL2 with

```
sudo apt install libsdl2-dev
```

Next, set up your build. Run the following commands in your shell in this order:

```
cd pVMpkin/
mkdir build
cd build/
cmake ..
make
```

Now, from your `build/` directory, you should be able to run any LC-3 compiled Assembly programs with pVMpkin. We've also provided a few demos (borrowed from [this 2048 repo](https://github.com/rpendleton/lc3-2048/tree/main) and [this Rogue-like repo](https://github.com/justinmeiners/lc3-rogue) in the `demos` directory.

To run pVMpkin, call its executable on a LC-3 compiled .obj file:

```
./src/pVMpkin  [.obj file path]
```

For example, to run the 2048 demo, use the following command:

```
./src/pVMpkin  ../demos/2048.obj
```

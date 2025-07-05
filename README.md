# ALGorithm GRAPH language

Is a graphical language (DSL) for scientific calculations and simulations.
The compiler converts the graph (encoded as xml)into C or an image.

## Build & Run

**Only the first time** you have to initialize the builder:

```shell
clang nob.c -o nob.exe
```

And then to compile the program run:

```shell
./nob.exe run
```

To see all possible commands run

```shell
./nob.exe help
```

> [!NOTE]
> All changes to the files will be detected, including nob.c, you need to use the compiler directly only once.

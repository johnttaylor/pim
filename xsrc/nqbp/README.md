NQBP stands for (N)ot (Q)uite (B)env - (P)ython.

NQBP is a multi-host build engine designed for:

    - C, C++, and assembler builds
    - Embedded development (but not limited to)
    - Minimal overhead to the developer, i.e. no makefiles required
    - Speed
    - Command line based
    - Supporting many Compiler toolchains
    - Source code resusablilty, i.e. NQBP assumes that code is/will be shared across many projects.
    - Resusablilty of Compiler toolchains, i.e once a particular compiler toolchain has been created/defined - it can be reused across an unlimited number of projects.
    - Very effective on small to medium size projects
    - Supports building in Outcast [http://www.integerfox.com/outcast]

NQBP is not:

    - A make based build engine. In fact NQBP does no dependency/what-has-changed checking at all!
    - An optimal choice for large (+100KLOC) projects 

Documentation is located in the top/ directory and/or availabe at: http://www.integerfox.com/nqbp.

NQBP is licensed with a BSD licensing agreement (see the top/ directory).

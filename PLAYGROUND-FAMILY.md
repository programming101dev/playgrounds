# p101 playground family

The playgrounds should be split by what students are learning, not by which tool
happens to detect the problem.

## `p101-c-playground`

Core C language correctness and security:

- heap ownership and pointer lifetime;
- `realloc` ownership;
- NULL and initialization checks;
- arrays, strings, bounds, and byte counts;
- integer overflow, underflow, conversion, and truncation;
- parsing and format strings;
- safe logging and parser fuzzing.

In this transition repository, run the C slice with:

```sh
./lab.sh --track c
./corpus.sh --track c
```

## `p101-systems-playground`

POSIX/system behavior:

- file descriptor ownership;
- file I/O short reads/writes, EOF, and interrupted system calls;
- file modes, symlinks, temporary files, and TOCTOU;
- fork/exec/wait and descriptor inheritance;
- signals, synchronization, threads, and multiprocessing;
- OS-resource cleanup order and resource limits.

In this transition repository, run the systems slice with:

```sh
./lab.sh --track systems
./corpus.sh --track systems
```

## `p101-network-playground`

Network behavior belongs in a separate playground, not in the C or systems
playground:

- TCP stream framing;
- UDP datagram size, truncation, loss, and reorder;
- socket-specific nonblocking I/O;
- address resolution, interfaces, IPv4/IPv6, byte order;
- connection lifecycle, timeouts, backpressure, and protocol parsing;
- the simple port forwarder as a capstone.

There are intentionally no `network` track lessons in this repository.

## Role of `p101-tool-playground`

`p101-tool-playground` remains useful as the meta/demo playground while the split
is being staged. It exercises the whole p101 toolchain and can generate either
track-specific lab books or the full combined book.

Once `p101-c-playground` and `p101-systems-playground` exist as separate repos,
this repository can either become the cross-tool demo or be retired.

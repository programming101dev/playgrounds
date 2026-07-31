# Descriptor inherited across exec

This case opens a file descriptor and reaches a recorded successful exec
boundary while that descriptor is still open and not marked `FD_CLOEXEC`. The
playground records the boundary without replacing its own process so it can
finish the event stream. A separate regression case proves that a failed exec
emits `EXECFAIL` and correctly cancels this finding.

That is a security bug because file descriptors are capabilities. A child
program can inherit a private file, socket, pipe, or listening descriptor that
it was never supposed to hold.

## What to look for

- A descriptor opened before `p101_execv`, `p101_execve`, or `p101_execvp`.
- No close before the exec boundary.
- No `FD_CLOEXEC` flag on descriptors the child should not inherit.

## Fix path

1. Decide whether the descriptor is intentionally part of the child process.
2. If not, mark it close-on-exec after opening it:

```c
p101_fcntl(env, err, fd, F_SETFD, FD_CLOEXEC);
```

3. Or close it before calling an exec wrapper.
4. Re-run `./lab.sh`.

This lab is fixed when `P101-FD-004` disappears for `exec-inherit`.

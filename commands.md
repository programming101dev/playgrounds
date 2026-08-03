# p101-tool-playground commands

Quick reference for the playground and the tools it is meant to show.

## Program scenarios

| Command | What it does |
| --- | --- |
| `p101-tool-playground` | Run the clean tour |
| `p101-tool-playground -s clean-file` | Clean file descriptor + allocation path |
| `p101-tool-playground -s realloc` | Allocate, grow, and free |
| `p101-tool-playground -s pipe` | Pipe read/write/close path |
| `p101-tool-playground -s fork` | Forked child resource path |
| `p101-tool-playground -s fd-leak` | Intentional descriptor leak |
| `p101-tool-playground -s alloc-leak` | Intentional allocation leak |
| `p101-tool-playground -s double-close` | Intentional double close |
| `p101-tool-playground -s stray-close` | Intentional bad close |
| `p101-tool-playground -s fault-lab` | Clean unless p101 fault injection trips setup |

## Runtime tools

| Command | What it shows |
| --- | --- |
| `p101 run -- p101-tool-playground -s tour` | Capture, model, and analyze a clean run |
| `p101 run -- p101-tool-playground -s fd-leak` | Descriptor leak analysis |
| `p101 resource analysis-dir` | Resource policy view |
| `p101 resource -j analysis-dir` | Machine-readable resource policy |
| `p101 trace analysis-dir` | Call tree |
| `p101 trace -s analysis-dir` | Call summary |
| `p101 report analysis-dir` | Correlated resource + trace narrative |
| `p101 report -j analysis-dir` | Correlated JSON |
| `p101 walk -- p101-tool-playground -s fault-lab` | Injected error-path resource audit |

## Project quality tools

| Command | What it does |
| --- | --- |
| `./tour.sh` | One-command full playground artifact |
| `./tour.sh --skip-quality --skip-coverage -n 5` | Fast runtime-only tour |
| `./lesson.sh wrappers` | Focused wrapper-boundary lesson |
| `./lesson.sh fd-leak` | Focused resource-leak lesson |
| `./lesson.sh error-path` | Focused injected-error-path lesson |
| `./lesson.sh module-split` | Focused module-shape lesson |
| `./change-compiler.sh -c clang` | Configure the build |
| `./build.sh` | Strict analysis build |
| `./test.sh` | Unity tests |
| `./fuzz.sh -t 10` | Parser fuzz smoke |
| `./coverage-report.sh --no-open --min 50 -- -s tour` | Coverage report + threshold |
| `./check.sh` | Format + build + tests + fuzz gate |
| `./doctor.sh` | Report local tool availability |
| `./clean.sh` | Remove generated build/report artifacts |

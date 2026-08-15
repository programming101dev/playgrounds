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
| `p101-inspect run -- p101-tool-playground -s tour` | Capture, model, and analyze a clean run |
| `p101-inspect run -- p101-tool-playground -s fd-leak` | Descriptor leak analysis |
| `p101-inspect view resource analysis-dir` | Resource policy view |
| `p101-inspect view sync analysis-dir` | Synchronization policy view |
| `p101-inspect view trace analysis-dir` | Call tree |
| `p101-inspect view report analysis-dir` | Correlated resource + trace narrative |
| `../scripts/runtime/student-workflow.sh -- p101-tool-playground -s fault-lab` | Injected error-path resource audit |

## Project quality tools

| Command | What it does |
| --- | --- |
| `./tour.sh` | One-command full playground artifact |
| `./tour.sh --skip-quality --skip-coverage -n 5` | Fast runtime-only tour |
| `./lesson.sh wrappers` | Focused wrapper-boundary lesson |
| `./lesson.sh fd-leak` | Focused resource-leak lesson |
| `./lesson.sh error-path` | Focused injected-error-path lesson |
| `./lesson.sh module-split` | Focused module-shape lesson |
| `cmake -S . -B build -DCMAKE_C_COMPILER=clang -DP101_BUILD_LEVEL=1` | Configure the build |
| `cmake --build build` | Strict analysis build |
| `cmake -S . -B build -DP101_BUILD_LEVEL=2 && cmake --build build` | Unity tests |
| `configure and run the fuzz/ CMake project -t 10` | Parser fuzz smoke |
| `configure with -DP101_COVERAGE_MODE=ON and run gcovr --no-open --min 50 -- -s tour` | Coverage report + threshold |
| `cmake -S . -B build -DP101_BUILD_LEVEL=3 && cmake --build build` | Format + build + tests + fuzz gate |
| `cmake -S . -B build` | Report local tool availability |
| `cmake --build build --target clean` | Remove generated build/report artifacts |

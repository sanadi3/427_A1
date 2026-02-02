# Assignment 1 - Shell Project

This project was developed using the starter code provided.

## Project Structure

```
427_A1/
├── project/
│   └── src/
│       ├── shell.c          
│       ├── shell.h           
│       ├── interpreter.c     
│       ├── interpreter.h     
│       ├── shellmemory.c     
│       ├── shellmemory.h     
│       ├── Makefile          
│       └── mysh              
├── test-cases/
│   ├── test_all.sh          # Automated test script
│   ├── *.txt                # Test input files
│   └── *_result.txt         # Expected output files
├── starter-code/            # Original starter code
└── README.md                
```

## Building the Project

From the `project/src` directory:
```bash
make clean
make mysh
```

This compiles `shell.c`, `interpreter.c`, and `shellmemory.c` into the `mysh` executable.

## Running Tests

### Run All Tests Automatically

Navigate to the `test-cases` directory and run:
```bash
chmod +x test_all.sh
./test_all.sh
```

### Run a Single Test

From the `test-cases` directory:
```bash
../project/src/mysh < echo.txt | diff - echo_result.txt
```

## Implementation Details

The project/src folder contains the implementation. The following files were modified from the starter code:
- `shell.c` - Fixed batch mode handling and semicolon-separated command parsing
- `interpreter.c` - Added echo, my_ls, my_mkdir, my_touch, my_cd, and run commands

## Team

- Team Member 1: Sanad Abu Baker, 261143797
- Team Member 2: Nigel Yip, 261308092

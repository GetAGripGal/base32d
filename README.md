A simple base32 decoder I wrote because I was sick of there not being one on windows and macos.

# Limitations

Only really works for strings at the moment.

# Building

The compiler is gcc.

```bash
make            # Build the project with gcc.
CC=clang make   # Build the project using clang.
```

# Usage

Redirect the output of echo to base32.

```bash
echo <base32_string> | base32d
```

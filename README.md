# Lama Bytecode Frequency Analyzer

The Lama Bytecode Frequency Analyzer is a static analysis tool designed to calculate the frequency of bytecode occurrences within Lama programming language programs.

This project employs the c-hashtable project for efficient hash table implementation. Visit [c-hashtable](https://github.com/davidar/c-hashtable) for more details on the original project.

## Building

To build the project, use the following command:

```bash
make all
```

## Usage

After building the project, run the analyzer using the following command:
```bash
./build/freq_analyzer <input_file.bc>
```

Replace <input_file.bc> with the actual bytecode file you want to analyze.

Example:
```bash
./build/freq_analyzer Sort.bc
```

## Cleaning
To clean up the build artifacts and executable, use:

```bash
make clean
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

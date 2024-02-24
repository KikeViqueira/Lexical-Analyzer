# Lexical-Analyzer

The purpose of this practice is to develop a lexical analyzer for Python code, focusing on analyzing a specific script named `wilcoxon.py`. A lexical analyzer, or lexer, is a critical component in compilers and interpreters, tasked with reading source code and breaking it down into a sequence of tokens. These tokens represent the atomic elements of the language, such as keywords, identifiers, numeric constants, operators, etc.

This project aims to apply theoretical concepts of lexical analysis in a practical scenario, involving the creation of tools capable of processing input text (Python source code) and extracting tokens based on the lexical rules of Python. It requires understanding these rules and implementing deterministic finite automata (DFA) to identify different token types from character sequences.

Key components of the project include:

1. **Definitions File**: Contains constants representing different recognizable tokens in `wilcoxon.py` and defines the symbol table structure for storing identified tokens.
2. **Lexical Analyzer**: The core of the project, responsible for reading source code and extracting tokens using automata to match character patterns.
3. **Symbol Table**: Manages identified tokens to avoid duplication and facilitate token search and reference.
4. **Input System**: Handles source code reading, providing characters to the lexer and managing the input flow efficiently.
5. **Error Handling**: Manages errors during lexical analysis, providing useful debugging and correction information.
6. **Main Program (Driver)**: Invokes the lexer and displays identified tokens, serving as an evaluation and testing tool.

This project allows computer engineering students to apply theoretical compiler knowledge to a practical project, enhancing their understanding of lexical analysis principles and programming language structure, specifically Python. The implementation of this lexer not only contributes to technical learning but also improves programming and software design skills.

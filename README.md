# DIRLOC (Directory Lines Of Code)
Count the number of ACTUAL lines of code (LOC) in a directory, excluding things like comments, whitespace, and semantic trivialities that do not contribute to the actual operation of your code. While it defaults to reading C/C++ files, support for other languages is in the works. 

## Usage/Features:

(NOTE: if using multiple options, they **must** be supplied in alphabetical order. If it is used, the "-e <path>" argument **must** be the last one in the command.)
| **Options** | **Description** | **Notes** |
| :---: | :--- | :--- |
| ```-c, -c-only``` | Only queries the C language's fileset | These files are also queried with the default behavior (C++ fileset). |
| ```-e, -ext,```<br>```-external <path>``` | Specify an external directory to search for code files | ***IF USED, MUST BE THE LAST ARGUMENT IN THE COMMAND.*** Allows both *relative* and *absolute* paths. |
| ```-g, -graph``` | Prints a "graph" representing the # of LOC, relative to the file with the most LOC in each file type | The LOC graph is marked with "#"; the skipped line graph is marked with "$". |
| ```-s, -skips``` | Prints the number of "undesireable" lines which were skipped while counting the file's LOC | This can be used in conjunction with any other option. |
| ```--h, --help``` | Prints a help message which contains these arguments and their uses | |
| ```--v,```<br>```--version``` | Prints the version of DIRLOC you have installed | |
| ~~```--gui```~~ | ***[not yet implemented]*** *Launches the program in GUI mode* | *This will have some prettier-looking graphics and methods of comparison.* |
| ```-i,```<br>```-ignore <ext1,ext2, ...>``` | | |

### Currently supported filesets (more coming soon, taking suggestions):

| **Language** | **Option** | **File set** |
| :----------: | :--------: | :----------: |
| C++          | (default behavior) | *.cpp, *.cxx, *.cc, *.C, *.ixx, *.cppm,<br> *.hpp, *.hxx, *.hh, *.h, *.H |
| C            | "-c", "-c-only" | *.c, *.h |
| Python       | *coming soon* | *coming soon, probably just \*.py<br>but we shall see...* |

## Build requirements:
- Qt 6.9.1 libraries. *This dependency will be refactored soon, it is just a byproduct of my development environment.*
- A compiler that supports C++20 (GCC >= v13, clang >= v17, MSVC >= v16.11)
- CMake >= v3.19


## How to build:
1. Clone this repository to your machine, or download it as a .zip and extract the contents to a folder.
2. In the terminal, enter the directory in which you cloned or downloaded the repository.
	
 	```cd ./dirloc```
3. Create a directory named "build" and then enter it.
	
 	```mkdir build && cd ./build```
4. Run cmake to build the program. The executable should appear in "dirloc/build"
	
 	```cmake -S .. -B . --target all```


## Examples of ignored "trivialities" (C/C++):
(note: these examples are written in C++, but their rules also apply to C programs)


**Single- and multi-line comments**
```
	 ...
1    // This line does the thing
2    int n = do_the_thing();
3    /* I need to fix this stuff:
4     *    - project file structure
5     *    - gui classes
6    */ 
	 ...
```
- In this example, line #1 and lines #3 through #6 will not count toward the Total Lines Of Code (TLOC) since they are comments

**Lines of whitespace (empty lines, newlines)**
```
1    #include <iostream>
2    int main() {
3        std::cout << "Interesting string\n";
4
5        return 0; }
```
- In this example, line #4 will not be counted towards the TLOC because it is empty.

**Closing brace, closing brace followed by semicolon**
```
(dirloc.h)
1    class dirloc
2    {
3        public: 
4            dirloc(int n);
5    };

(dirloc.cpp)
...
6    dirloc::dirloc(int n) {
7        if (n == 1) {
8            return;
9        }
10   }
...
```
- In this example, line #2 and line #5 in ```dirloc.h``` will not count toward the TLOC. Similarly, line #9 and line #10 in ```dirloc.cpp``` will not count
- These braces are essential for the structure of the program, but they do not contribute to the execution of the code


## Purpose:
This tool exists purely for fun; it isn't meant to give any legitimate qualitative insight into your code. What it IS meant for is to give the programmer some insight into how the length of their code is changing throughout the program's development, and evoke some reflection on the amount of "work" they have done. This measure is purely relative to each programmer, and the satisfaction you extract from the numbers growing or shrinking is up to you.


Another concept this tool might help with is the notion of splitting your code into multiple files. The usefulness and feasibility of this concept varies across languages, but in general it is a wise practice to keep your files from growing too large in scope and size.


In general, the number of lines of code does NOT reflect the quality, complexity, or correctness of your program. A very long program might just be an over-engineered implementation of something very simple, and a very short program might do some convoluted multi-threaded operation that is abstracted away in another library.


I for one take some satisfaction in seeing how the files in a project of mine grow and change as I flesh out more and more features that I have planned. On one day, it might be amusing to see how large some of my source files get as I insert more functionality. On another day, it might be satisfying to see the opposite, as I work on optimizing and shrinking a particularly verbose implementation.

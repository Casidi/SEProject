# SEProject
The Software Engineering term project in fall 2016.

## How to build
### Required Files
* All files on github
* [mysql.zip](https://drive.google.com/file/d/0B5SEfPKgXclXRDdRX1huTjF6M2s/view?usp=sharing)
* [googletest.zip](https://drive.google.com/open?id=0B5SEfPKgXclXMjViQ085cDZZaEk)

### Steps
1. Extract mysql.zip and googletest.zip to the project folder.
2. Open the SEProject.sln with Visual Studio.
3. Press F7 to build project.

## About the code style
### Naming
I only use two styles for all source files.
#### Style 1
For function names, variable names
Code:
```c++
void thisIsTheFunction();
int thisIsTheVariable;
```
#### Style 2
For class names, struct names
Code:
```c++
class ThisIsAClass;
struct ThisIsAStruct;
```

### Indentation
There is no rules for indentation. The code below is just for reference.
```c++
#include <iostream>

using namespace std;

int countFactorial(int theInputNumber) {
  int result = 1;
  for(int i = 0; i < theInputNumber; ++i)
    result *= i + 1;
  return result;
}

int main(int argc, char* argv[]) {
  cout << "This is an example of ultra long line of text, or a long text segment." << endl
    << "It's recommended to seperate deferent lines in source codes." << endl
    << "This is the last line." << endl;
    
  int theMagic = 1234;
  switch(theMagic) {
  case firstCase:
    cout << "This is the first case." << endl;
    break;
  case secondCase:
    cout << "This is the second case." << endl;
    break;
  default:
    cout << "This is the default case." << endl;
    break;
  }
  
  return 0;
}
```

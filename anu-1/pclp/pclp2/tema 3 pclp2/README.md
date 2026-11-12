# Homework 3 - Going home bit by bit

**Authors**
 - Alexandru RĂDUȚĂ
 - Robert-Andrei POPESCU

**Team leader**
- Ilinca-Ioana STRUŢU
---

### DEADLINE SOFT: 21.05.2026 23:59
### DEADLINE HARD: 26.05.2026 23:59

---

### Time to go home with Eli and Edi

**Eli reminds you that the homework should be solved on the PCLP2 virtual machine. Edi advises you that even if you are using a different system (native Linux, WSL, etc) you should still check your solution on the PCLP2 virtual machine**

---

It is time for our friends to continue their journey across Europe. After visiting the fast moving world of Formula 1, and the calm gardens of Versailles and everything in between, Eli and Edi are exhausted.

Their bags are full and batteries are empty after their adventures, but they still have one more stop before arriving back home to the bug infested dorm back in Regie.

They had just boarded the plane to go to the wonderful beaches of Greece, but got rerouted due to bad weather and reached Poland's capital, Warsaw.

## Task 1 - My Vector (20p)

<div align="center">
    <img title="IDS" alt="IDS" src="./src/images/vector.jpg" width=auto height=auto>
</div>

### The Story

As they reach Warsaw and start buying more souvenirs, they start realising they have bought way too many. The two of them want to stop using a simple bag with a fixed size for every items, and instead want to use a modular bag, that can change sizes according to their needs.

They decide that the best bag they can use is a vector, in which they can easily add or remove items, and that will automatically resize based on how many items they currently have. 

### Problem Statement

You have to implement the following structure and functions:

```c
struct vector {
	int *arr;
	int len;
	int cap;
}

struct vector *new_vector(int cap);
int set_element(struct vector *vec, int elem, int pos);
int get_element(struct vector *vec, int pos);
int push_element(struct vector *vec, int elem);
int pop_element(struct vector *vec);
void print_vector(struct vector *vec);
void free_vector(struct vector **vec);
```

The vector structure is composed of a pointer to a dynamically allocated array, the length of the array, and its capacity. The functions have the following behaviour:

`new_vector`: Allocates a vector on the heap, sets the fields and returns it's address.

`set_element`: Sets the element elem at position pos, if the position is smaller than the length of the array. Returns the position if the position is valid, or -1 otherwise.

`get_element`: Returns the element at position pos, if the position is smaller that the length of the array, or -1 otherwise.

`push_element`: Inserts the element elem at the end of the array, and doubles the array capacity if necessary. Returns the position where the element was inserted.

`pop_element`: Returns the element at the end of the array, removes it from the array, and halves the capacity if possible. If the array has no elements, return -1.

`print_vector`: Prints the content of the vector to stdout.

`free_vector`: Frees the whole structure, including the internal array.


### Constraints

The error prone functions (`set_element`, `get_element` and `pop_element`) need to print to stdout their errors.

-1 will not be introduced, it is reserved for errors.

The capacity should never go below 1, even if the array is empty.

Pop should only halve the array if it removes an element.

### Examples

``` c
v = new_vector(2) // returns v -> {([][]), 0, 2}
print_vector(v); // prints "v -> {([][]), 0, 2}"
get_element(v, 1) // returns -1, prints "Error: len <= 1"
set_element(v, 0, 1) // returns -1, prints "Error: len <= 1"
pop_element(v) // returns -1, prints "The vector is empty"
push_element(v, 1) // returns 0
print_vector(v) // prints "{([1][]), 1, 2}"
push_element(v, 2) // retuns 1
print_vector(v) // prints "v -> {([1][2]), 2, 2}"
push_element(v, 3) // returns 2
print_vector(v) // prints "v -> {([1][2][3][]), 3, 4}"
set_element(v, 1, 1) // returns 1
print_vector(v) // prints "v -> {([1][1][3][]), 3, 4}"
get_element(v, 2) // returns 3
pop_element(v) // returns 3
print_vector(v) // prints "v -> {([1][1]), 2, 2}"
get_element(v, 2) // returns -1, prints "Error: len <= 2"
free_vector(&v) // frees the vector and sets it as NULL
```

## Task 2 - Reverse Polish Notation (20p)

<div align="center">
    <img title="IDS" alt="IDS" src="./src/images/rpn.jpg" width=auto height=auto>
</div>

### The Story

Now that our friends can hold as many items as they want they start asking the real question. How much money have they spent on this whole adventure. They start adding each item by hand, getting lost in who paid what, how much an item was discounted, and how much of each item they bought.

Luckily, their answer came from a place they expected the least, The National Museum of Technology. They saw an exhibition on ways to compute large numbers throughout history. From the first abacus developed by the Sumerians, to today's supercomputers. But it wasn't the supercomputers who caught their eyes, but something much more local.

They saw that in 1924 a Polish mathematician invented a way to write complex algebra without using parentheses, rather he wrote the calculation by writing the operand before the operators. Instead of 1 + 2, he wrote + 1 2.

In the 1950's and 1960's this Polish Notation was adapted to work better for stack based computation in the early computer era, in which the order was reversed, naming it Reverse Polish Notation.

### Problem Statement

You have to implement a program that evaluates mathematical expressions written in Reverse Polish Notation (RPN), also known as postfix notation. In RPN, operators follow their operands. For example, to add 3 and 4, you would write 3 4 + rather than the standard 3 + 4.

To evaluate these expressions efficiently, you will need to use the built in stack.

Your program must read the input from the standard input (stdin). The input will consist of numbers and operators read as strings, with each element separated by a newline character (\n). Whenever you read a number, you convert it to a 64-bit integer and push it onto the stack. Whenever you read an operator, you pop the required number of operands from the stack, perform the operation, and push the result back onto the stack.

At the end of the input, the final result will be the only remaining element in your vector. You must print this result to the standard output (stdout).

### Constraints

The input elements (operands and operators) will be provided one per line (separated by \n). You should read them as strings until EOF (End Of File).

Your program should support the basic arithmetic operators: + (addition), - (subtraction), * (multiplication), and / (integer division).

Pay close attention to the order of operations for subtraction and division! If you pop B then A, the operation is A - B or A / B.

You can assume that the input is always a valid RPN expression and there will be no division by zero.

All intermediate and final calculations will fit within a standard 64-bit signed integer (e.g., long on 64-bit systems, or int64_t). Make sure you use the appropriate functions for string-to-number conversion (like atol instead of atoi).
The final output must be printed directly to stdout, followed by a newline character.

### Examples

```c
Input:
20
30
+

Output:
50

Explanation:

Read "20", convert to 64-bit int, push to stack. Stack: [20]

Read "30", convert to 64-bit int, push to stack. Stack: [20, 30]

Read "+", pop 30, pop 20. Calculate 20 + 30 = 50. Push 50. Stack: [50]

EOF. The final result is 50.
```

```c
Input:
5
1
2
+
4
*
+
3
-

Output:
14

Explanation:
This is the postfix equivalent of the infix expression: 5 + ((1 + 2) * 4) - 3.
1. Push 5. Stack: [5]
2. Push 1. Stack: [5, 1]
3. Push 2. Stack: [5, 1, 2]
4. Read "+". Pop 2, pop 1. Calculate 1 + 2 = 3. Push 3. Stack: [5, 3]
5. Push 4. Stack: [5, 3, 4]
6. Read "*". Pop 4, pop 3. Calculate 3 * 4 = 12. Push 12. Stack: [5, 12]
7. Read "+". Pop 12, pop 5. Calculate 5 + 12 = 17. Push 17. Stack: [17]
8. Push 3. Stack: [17, 3]
9. Read "-". Pop 3, pop 17. Calculate 17 - 3 = 14. Push 14. Stack: [14]
10. EOF. The final result is 14.
```

## Task 3 - Printing (20p)

<div align="center">
    <img title="IDS" alt="IDS" src="./src/images/printf.jpg" width=auto height=auto>
</div>

### The Story

Before heading to the airport to catch their flight home, they find out that the IT department at Warsaw Chopin Airport are having a scheduled maintenance and are not accepting digital tickets, only ones that are printed on paper.

In a hurry they enter a dusty antique store, where they find a vintage computer linked to a clunky dot-matrix printer. There is only one problem, the computer does not have a full standard C library install. The library does not have the function they are looking for, which is `printf()`, but it luckily has the `putc()` function, which can be used to write it.

### Problem Statement

You have to implement the following function:

```c
int my_printf(const char *format, ...);
```

Apart from the format, the function can take 0, 1 or up to n extra arguments. The function will only use the `putc` external function, using other functions is prohibited.

### Constraints

The extra arguments can only be unsigned integers (`%lu`), chars (`%c`) and strings(`%s`).

You can assume that the function will be called with the correct parameter types. There will be no strings at `%c`, floats at `%lu`, etc.

### Examples

```c
my_printf("Hello from %s, I like the number %lu%c\n", "Alexandru", 2, '!');
// prints "Hello from Alexandru, I like the number 2!"
```

## Task 4 - The Langford Sequence (30p)

<div align="center">
    <img title="IDS" alt="IDS" src="./src/images/langford.jpg" width=auto height=auto>
</div>

### The Story

After the little ticket panic, Eli and Edi catch their flight. While listing to the flight attendant's safety instructions, Eli notices something interesting about the flight number. 

Their number is 312132, which is a special number, as there are 3 digits between the two 3s, 2 digits between the two 2s and 1 digit between the two 1s. Edi tells her that this is a langford sequence, and she wants to try see if she can find more langford sequences to pass the time in the air.

### Problem Statement

You have to implement 2 functions: `check_langford` and `generate_langford_sequences`.

A langford sequence is a sequence formed from the sequence {1, 1, 2, 2, ..., n, n}, where there are k elements between each pair of k and k.

For example a langford sequence for {1, 1, 2, 2, 3, 3} is {2, 3, 1, 2, 1, 3}, as there is 1 element between the 1s, 2 elements between the 2s, and 3 elements between the 3s.

The number n is also known as the order of the langford sequence. {2, 3, 1, 2, 1, 3} is a langford sequence of order 3.

### Subtask 1 - Checking the sequence (5p)

Firstly, you need to know if a sequence is a Langford sequence. For that you will need to implement the following function:

```c
int check_langford(int *sequence, int len);
```

The function takes two arguments, the first one is the sequence itself, and the second one is its length, and the functions returns 1 if the sequence is a Langford sequence and 0 otherwise.

### Examples

```c
check_langford([1, 2, 3, 1, 2, 3], 6) // returns false, there are 2 elements between the two 1s and the two 3s

check_langford([3, 1, 2, 1, 3, 2], 6) // returns true, there is 1 element between the two 1s, two elements between the two 2s and three elements between the two 3s
```

### Subtask 2 - Generating the sequence (25p)

Now that you can check if a sequence is a Langford sequence, you will need to implement a function that generates them:

```c
int **generate_langford_sequences(int n, int *seq_num);
```

The function takes two arguments. That first argument is the order of the sequence, and the second argument is a pointer to an integer, which you need to set as the number of the generated sequences. The function will return all langford sequences of order k. If there are multiple langford sequences generated, return them sorted lexicographically.

### Constraints

All arrays will be dynamically alocated, the checker will free them.

If there are no generated sequences, you need to return NULL.

### Examples

```c
generate_langford_sequences(3, &n) // returns ([2, 3, 1, 2, 1, 3], [3, 1, 2, 1, 3, 2]) and sets n = 2.

generate_langford_sequences(5, &n) // returns () and sets n = 0, as there are no langford sequences for k = 5.
```

## Bonus Task - Welcome Home (20p)

<div align="center">
    <img title="IDS" alt="IDS" src="./src/images/bonus.jpg" width=auto height=auto>
</div>

### The Story

Touchdown! The plane lands at Aurel Vlaicu Airport in Bucharest, and our friends are almost home.

They take the bus back home to Regie, but when they enter their dorm, they find that they can't enter their rooms, as their roomates played a prank on them, and won't allow them to enter until they don't solve a set of puzzles for them.

### Problem Statement

The bonus points will be scaled with the rest of the assignment points (tasks 1 to 4 and coding style). For example if you have 80p on the main assignment and get 20p on the bonus task, your total score will be 20 * 80 / 100 + 80 = 96 points.

The bonus points can help towards points lost on other assignments, but not on any other activity.

You will need to implement the following functions: `ave`, `switch_cases`, `heavy` and `flat_matrix`.

### Subtask 1 - Ave

The first function is:

```c
void ave(char *src, char *dest, int n);
```

The function will parse the source string and it will change every character using this formula: `(c - 'A' + n) mod 31 + 'A'`, where 'A' is the ASCII code for 'A'.

The result is saved in the destination string, which is already alocated.

### Subtask 2 - Switch Cases

The second function is:

```c
void switch_cases(char *src, char *dest);
```

The function will parse the source string and it will change the capitalization of all letters.

The result is saved in the destination string, which is already alocated.

### Subtask 3 - Heavy

The third function is:

```c
void heavy(int number);
```

The function will check if the number given is heavy or not.

A number is heavy if the MSB (most semnificant bit) is set and the resulting number made from concatenating the 3rd and 4th byte of that number is bigger than 255.

If the number is heavy, print "The number is heavy" to stdout, and print "The number is not heavy" to stdout otherwise.

### Subtask 4 - Flat Matrix

The last function is:

```c
void flat_matrix(int *mat, int n);
```

Mat is a flattened n * n matrix, where the element which was originally at [i][j] is now saved at [i * n + j].

The function will print to stdout the biggest number from each column, separated by a new-line.

## Coding Style (10p)

To be able to use your solutions in the implementation of the secret algorithm, Eli needs well-structured and readable assembly code that follows a few good practice rules:

- writing readable code
- consistent indentation (the recommendation is to place labels at the beginning of the line and indent instructions with one tab)
- using meaningful names for labels
- including **relevant and necessary** comments in the code

## Checker

To use the checker you need to run `./checker/checker.sh`.

To get the actual grade you need to upload your zip archive on moodle (use `make pack` in `./src`)

## The adventure comes to an end

With their doors finally open, they can rest after their long journey. They open their laptop to see what awaits them in the next semester, and Eli can't wait to learn more about the computer she is using and how it works in the Operating Systems course, while Edi awaits the Algorithm Analysis course, to know how fast and efficient his code is.

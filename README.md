# skalang

![Example](https://github.com/Scorbutics/skalang/doc/skalang_script_example.gif)

## Motivation
### Why making another 'scripting' language ? Isn't Python / Ruby / TypeScript / LUA / AngelScript enough ?
Honestly, it all started by the requirement of my other project of builing a 2D game engine (named 'ska').
I needed a robust and fast developing scripting language with a C++ binding, and I wanted it to have some uncommon features, which are listed below.
 * Mainly, Python Ruby and LUA are (designed as) dynamic typed languages, and I just do not like it.
 * In my opinion, TypeScript is an awesome language in term of flexibility and compilation features. It also supports traits natively with its "interfaces", which allows duck typing.
I have no argument against the use of TypeScript, despite the lack of well-known implementations of C++ binding ?
 * AngelScript seems to have good features regarding static typing, but I do not want to use a strict C++-like syntax.

I wanted a simple but coherent language.

## Features
 * Static typing only
 * Currently, it is weakly typed (you can affect an integer to a string variable, it will be converted), but it will probably becomes strongly typed soon
 * Has a compilation phase (to bytecode), that includes type and semantic checks
 * Huge use of type inference
 * Variable-based syntax : functions are variables, import are variables, variables... are variables ;)
 * Object paradigm, without classes (uses a prototyping approach), without polymorphism (uses the function compatibility feature)
 * Only "one way to do a thing", as Python philosophy
 * No "null" value or uninitialized variables (memory safety)
 * No manual memory management (reference counting system), but doesn't support cyclic references
 * Includes a C++ binding

## Syntax
The syntax is a cross between Ruby and Javascript.
It can looks a little weird at first, but it is VERY consistent.
For example, syntax follows some major rules on its use of symbols and keywords :
 * ':' is for the typing system, it explicitly specify a type for an expression. Double it ( '::' ) and it refers to a script namespace.
 * '{' and '}' are for object instanciation (and is used after the 'return' keyword) only
 * 'do' and 'end' are for statement blocks
 * '(' and ')' have two uses, one for grouping expression (as in mathematics), and the other for function parameters definition and call
 * '|' is a pipe operator, in the same way as you can find in frameworks like Angular. It applies a lambda function on a collection.
 * '[' and ']' are used for array declaration and use

## Code examples
 * Basic parameters use

```ruby
IOLog = import "bind:std.native.io.log"
PathFcty = import "bind:std.native.io.path"

run = function (parameters: string[]) do
	if parameters.size() > 0
		path = PathFcty.Fcty(parameters[0])
		IOLog.print(path.canonical())

		all = []: int
		parameters | (parameter, index) do
			if index > 0
				all = all + (parameter: int + index * index)
			end
		end

		all | (element) do
			IOLog.print(element)
		end

		IOLog.print("size : " + all.size())

	else
		IOLog.print("No argument provided")
	end
end
```

 * Function type compatibility
```ruby
IOLog = import "bind:std.native.io.log"

Default = function() do end

call = function(deferedArray: Default[]) do
	deferedArray | (defered) do
		defered()
	end
end

run = function(parameters: string[]) do
	callback = function() do
		IOLog.print("defer test")
	end

	callback2 = function() do
		IOLog.print("defer test 2")
	end

	call([callback, callback2])
end
```
 * Object creation

 ```ruby
 Fcty = function(x: int, y: int): var do
	return {
		x = x
		y = y
	}
end
 ```


## Installation / Use
The project uses CMake to compile and a C++17 (at least) compiler.
It has **no** dependency excluding the C++ standard library, excluding my own skalog and skabase project, used respectively as logging lib and common programming tools.
It has been tested sucessfully on Windows (10), Linux (18.04) and on an Android (5+) based device with the Termux app to have an in-device terminal.

Clone this repository with the external git submodules :
`git clone --recurse-submodules https://github.com/Scorbutics/skalang`
Then you just have to start CMake and compile the project from the root folder :
`cmake .`
And
`cmake --build .`
As you might notice, the project is split up in several dynamic libraries, which are modules required by the skalang bytecode runner.
To run an example script :
`./bin/skalang_bytecode_runner example.miniska`
(If you build with MSVC, you will probably have to add a target folder (Debug / Release / RelWithDebInfo) after the "bin" folder)

## Contributing
Currently, I made this project alone.
I do not usually work on my projects with other people, but I am totally open to it !
To contribute, you can either :
- Create an issue in the GitHub issues page that describe what you want to change
- Implement a feature by forking this repo and making a "Pull Request" on GitHub
- Add some documentation in the wiki

## Code architecture
	Blocks
		- Tokenizer and parser (lib skalang)
		- Type building / semantic type checking (lib skalang)
		- Bytecode generation (lib skalang_bytecode_generator)
		- Serialization (lib skalang_bytecode_serializer)
		- Interpretation, by the virtual machine (VM) (lib skalang_bytecode_interpreter)
		- Runner (facultative executable script starter)
	
## Benchmarks
No benchmarks on the interpreter (VM) have been done for now (I aim to add some to compare runtime executions with other known major scripting languages)

# skalang

Basic scripting language, syntax inspired from Ruby, JavaScript and one or two ideas picked-up from other scripting languages.

### Features
* Has a compilation phase ("one pass" to bytecode)
* Static typing
* Object paradigm (classes, no polymorphism)
* No "null" value or uninitialized variables (memory safety), but accessing an array out of bounds is still possible
* C++ binding
* No manual memory management (reference counting system), but doesn't support cyclic references
* Only one way to do a thing
* Variable-based syntax : functions are variables, import are variables, variables... are variables ;)

### Example

```javascript
Foo = function(): var do 
	return {
		data = 500
	}
end

Bar = function(): var do
	power = 88
	return {
		attack = function (target: Foo()) do
			target.data = target.data - power
		end
	}
end 

foo = Foo()
Bar().attack(foo)

Runner = import "runner_lib"
Runner.print(foo)

```

This code displays "412" in the console, because Runner.print prints the data field of a "Foo" instance.

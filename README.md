# skalang

Basic scripting language, syntax looking like Javascript.

### Features
* Has a compilation phase ("one pass" to bytecode)
* Static typing
* Object paradigm (classes, no polymorphism)
* No "null" value or uninitialized variables (memory safety), but accessing an array out of bounds is still possible...
* C++ binding
* No manual memory management (reference counting system), but doesn't support cyclic references
* Only one way to do a thing (loops are only made with "for")
* Variable-based syntax : functions are variables, import are variables, variables... are variables ;)

### Example

```javascript
var Coldragon = function(): var { 
	return { 
		data: 500
	}; 
};

var Scorbutics = function(): var {
	var power = 88;
	return {
		attack : function(target: Coldragon()) {
			target.data = target.data - power;
		}
	};
}; 

var coldragon = Coldragon();
Scorbutics().attack(coldragon);

var Runner = import "runner_lib";
Runner.print(coldragon);

```

This code displays "412" in the console, because Runner.print prints the data field of "Coldragon".

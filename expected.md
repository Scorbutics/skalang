_____________________________
Source:
_____________________________
makeCounter = function (init: int): var do
	count = 0
	return {
		counter = function(): int do
			count = count + 1
			return count
		end
	}
end

run = function(parameters: string[]) do
	v1 = makeCounter(10)
	v2 = makeCounter(10)
end
_____________________________
Expected bytecode:
_____________________________

[JUMP_REL|4||]
    // Pop from stack in "init"
    [POP|V0:4||]
    // Direct move 0 value in "count"
    [MOV|V1:4|0|]

    [JUMP_REL|4||]
        // "count" is detected to be in upper function, so we use "count" from env (closure) at index "0"
        // it is placed on the stack
        [USE_ENV|V2:4|0]
        // Pop the stack variable "count" into R0
        [POP|R0:4|]
        // Add 1 to "count"
        [ADD_I|R0:4|1]
        // Return "count"
        [RET|R0:4|]
    [END|V2:4|-5|]

    // Declare "count" to be in closure V2 env as env variable index "0"
    [ADD_ENV|V2:4|V1:4]

    // Return "counter"
    [RET|V1:4||]
// Declare this previous -11 lines block as function "makeCounter"
[END|V3:4|-11|]

[JUMP_REL|12||]
    // Pop from stack "parameters"
    [POP|V3:4||]
    // Add 10 to stack
    [PUSH|10||]
    // Call "makeCounter" with 10 in stack
    [JUMP_ABS|V3:4||]
    // Pop result into tmp register
    [POP|R0:4||]
    // Move this result into "v1"
    [MOV|V4:4|R0:4|]
    // Add 10 to stack
    [PUSH|10||]
    // Call "makeCounter" with 10 in stack
    [JUMP_ABS|V3:4||]
    // Pop result into tmp register
    [POP|R1:4||]
    // Move this result into "v2"
    [MOV|V5:4|R1:4|]
    // No value returned
    [RET|||]
// Declare this previous -13 lines block as function "run"
[END|V6:4|-13|]

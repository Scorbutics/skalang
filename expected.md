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
	v2 = makeCounter(20)
end
_____________________________
Expected bytecode:
_____________________________

[JUMP_REL|14||]
    // Pop from stack in "init"
    [POP|V0:4||]
    // Direct move 0 value in "count"
    [MOV|V1:4|0|]

    // counter start body
    [JUMP_REL|4||]
        // counter execution body start

        // "count" is detected to be in upper function, therefore "counter" is a closure and "count" is captured
        [USE_ENV|V1:4||]

        // Add 1 to "count"
        [ADD_I|V1:4|V1:4|1]
        // Return "count"
        [RET|V1:4|]

        // counter execution body end

        // As this function body contains a ref to "count" (V1:4), internally add V1:4's __value__ in the capture environment of "counter" (V2:4)
        [ADD_ENV|V2:4|V1:4|]
    // Declare this previous -3 lines block as function "counter"
    // /!\ "counter" is a closure because its capture environment is not empty
    [END|V2:4|-3|]
    // counter end body

    // Push the "counter" closure on the stack
    [PUSH|V2:4||]

    // Create an object from the stack (containing the "counter" closure)
    [POP_IN_VAR|R1:4|1|]

    // Return the object "counter"
    [RET|R1:4||]
// Declare this previous -11 lines block as function "makeCounter"
[END|V3:4|-11|]

[JUMP_REL|12||]
    // Pop from stack "parameters"
    [POP|V3:4||]
    // Add 10 to stack
    [PUSH|10||]
    // We've detected during bytecode gen. that the following "makeCounter" function (V3:4) is a closure (so needing a capture environment)
    // Therefore we've added a LOAD_ENV instruction
    // Call "makeCounter" with 10 in stack
    [JUMP_ABS|V3:4||]
    // Pop result into tmp register
    // it contains a "counter" closure with a node value of "10" inside the captured env "count" variable
    [POP|R0:4||]
    // Move this result into "v1"
    [MOV|V4:4|R0:4|]
    // Add 20 to stack
    [PUSH|20||]
    // Call "makeCounter" with 20 in stack
    [JUMP_ABS|V3:4||]
    // Pop result into tmp register
    // it contains a "counter" closure with a node value of "20" inside the captured env "count" variable
    [POP|R1:4||]
    // Move this result into "v2"
    [MOV|V5:4|R1:4|]
    // No value returned
    [RET|||]
// Declare this previous -13 lines block as function "run"
[END|V6:4|-13|]

_____________________________
Actual Bytecode (misses the ADD_ENV and USE_ENV parts):
_____________________________
[JUMP_REL|14||]
    [POP|V0:4||]
    [MOV|V1:4|0|]
    [JUMP_REL|5||]
        [ADD_I|R0:4|V1:4|1]
        [MOV|V1:4|R0:4|]
        [MOV|R1:4|V1:4|]
        [CLEAR_RANGE|R1:4|R1:4|]
        [RET|R1:4||]
    [END|V2:4|-6|]
    [PUSH|V2:4||]
    [POP_IN_VAR|R2:4|1|]
    [CLEAR_RANGE|R1:4|R2:4|]
    [CLEAR_RANGE|V0:4|V2:4|]
    [RET|R2:4||]
[END|V3:4|-15|]
[JUMP_REL|12||]
[POP|V4:4||]
[PUSH|10||]
[JUMP_ABS|V3:4||]
[POP|R3:4||]
[MOV|V5:4|R3:4|]
[PUSH|10||]
[JUMP_ABS|V3:4||]
[POP|R4:4||]
[MOV|V6:4|R4:4|]
[CLEAR_RANGE|R3:4|R4:4|]
[CLEAR_RANGE|V4:4|V6:4|]
[RET|||]
[END|V7:4|-13|]
________________________________

Idée pour générer ces instructions manquantes :

Pour la partie détection de closure ou non:
Exemple pour "count" avec le code source au début de ce fichier:
On arrive sur le node binary: "count + 1":
    => "count"
        => symbole associé
            => "Remonter" les nodes dans l'AST jusqu'à tomber sur un / une FUNCTION_DECLARATION / FACTORY_DECLARATION ou le root:
                => prendre la symbol table: on tombe sur la symbol table associée à "counter".
            => récupérer la symbole table associée à "count"
            => on compare les deux symbol tables, si identiques => pas de closure, sinon => closure
    => "1"
        => pas de symbole associé => pas de traitement

Note: à la place de remonter les nodes dans l'AST, on peut dans le GenerationContext, tout le temps garder la dernière symbol table qui fait référence à une FUNCTION_DECLARATION / FACTORY_DECLARATION.
ça évite le double parcours et simplifie l'algorithme


Pour la partie génération de bytecode:
Je suis dans une closure et j'utilise une variable (querySymbolOrOperand) avec un symbole associé:
    => si cette variable fait partie de la closure, je la récupère depuis l'env
    => sinon je la récupère depuis les paramètres de la fonction ou le scope courant contenu dans la fonction
    => je dois "tagger" la fonction comme étant une closure en ajoutant cette variable à la liste de dépendances dans l'environnement
    => car au moment de l'appel à cette fonction, je vais devoir construire un environnement associé à cette fonction

<?php

// Erreur syntaxique intentionnelle (manque de point-virgule)
echo "This is a test script with errors"

// Appel de fonction inexistante
nonExistentFunction();

// Utilisation d'une variable non définie
echo $undefinedVariable;

// Division par zéro
$result = 10 / 0;

// Erreur de logique (condition toujours vraie)
if (1 == 1) {
    echo "This condition is always true, but it's not an error.";
}

// Erreur de type
function addNumbers($a, $b) {
    return $a + $b;
}
echo addNumbers("string", 10);

// Cette ligne sera exécutée même après les erreurs ci-dessus
echo "Script execution continues despite errors.";
?>
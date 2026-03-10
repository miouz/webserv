# Contributing to webserv

Bienvenue ! Ce document décrit les conventions et le workflow à suivre pour contribuer au projet **webserv**.

---

## Table des matières

- [Workflow Git](#workflow-git)
- [Messages de commit](#messages-de-commit)
- [Conventions de nommage C++98](#conventions-de-nommage-c98)
- [Code Review & Pull Requests](#code-review--pull-requests)
- [Issues & tickets](#issues--tickets)

---

## Workflow Git

Nous utilisons un modèle **trunk-based** : une branche `main` stable, et des branches de feature éphémères.

### Règles

- `main` est toujours en état **compilable et fonctionnel**. On ne pousse jamais directement dessus.
- Chaque nouvelle fonctionnalité ou correction passe par une branche dédiée.
- Les branches doivent être **courtes** : idéalement mergées en moins de 2-3 jours.

### Nommage des branches

```
<name>/<type>/<description-courte>
```

Exemples :
```
mirate/feat/parse-http-request
jeremirate/fix/socket-leak
camirate/refactor/response-builder
mirate/docs/update-readme
```

Types valides : `feat`, `fix`, `refactor`, `docs`, `test`, `chore`

### Cycle de vie

```
main
 └── feat/ma-feature
      ├── commits...
      └── PR → review → merge → suppression de la branche
```

---

## Messages de commit

Nous suivons la spécification [Conventional Commits](https://www.conventionalcommits.org/).

### Format

```
<type>(<scope optionnel>): <description courte en impératif>

[corps optionnel]

[footer optionnel : BREAKING CHANGE, closes #xx]
```

### Types

| Type       | Usage                                              |
|------------|----------------------------------------------------|
| `feat`     | Nouvelle fonctionnalité                            |
| `fix`      | Correction de bug                                  |
| `refactor` | Réécriture sans changement de comportement         |
| `docs`     | Documentation uniquement                          |
| `test`     | Ajout ou modification de tests                    |
| `chore`    | Tâches annexes (Makefile, dépendances, config...)  |
| `style`    | Formatage, espaces, point-virgules (pas de logique)|

### Exemples

```
feat(parser): add support for chunked transfer encoding

fix(socket): close fd on bind() failure
closes #12

docs: add CONTRIBUTING.md
```

### Règles

- Description en **anglais**, à l'impératif (`add`, `fix`, `remove`), sans majuscule, sans point final.
- Maximum **72 caractères** pour la ligne de titre.
- Un commit = **une seule responsabilité**. Ne pas mélanger fix et refactor.

---

// _camelCase ou camelCase_ pour les attributs privés (choisir et rester cohérent)

private:
    int _port;
    std::string _host;
```

### Constantes & macros

```cpp
// SCREAMING_SNAKE_CASE
const int MAX_CONNECTIONS = 128;
#define BUFFER_SIZE 4096
```

### Fichiers

```
// PascalCase pour les classes
HttpRequest.hpp
HttpRequest.cpp

// lowercase pour les utilitaires
utils.hpp
config_parser.cpp
```

---

## Code Review & Pull Requests

### Ouvrir une PR

1. S'assurer que le code **compile sans warning** (`-Wall -Wextra -Werror`).
2. Vérifier que les fonctionnalités existantes ne sont pas cassées.
3. Nommer la PR de la même façon que la branche : `feat/parse-http-request`.
4. Rédiger une description courte : **ce qui a été fait** et **pourquoi**.

### Processus de review

- Toute PR doit être approuvée par **2 autres membres** avant le merge.
- Le reviewer vérifie : lisibilité, conformité aux conventions, pas de fuite mémoire évidente, logique correcte.
- Les commentaires de review doivent être **constructifs et précis** (indiquer la ligne, proposer une alternative si possible).
- L'auteur répond à chaque commentaire avant de re-demander une review.

### Merge

- On utilise **squash merge** ou **merge commit** (à définir en équipe), pas de fast-forward nu.
- La branche est supprimée après le merge.
- Ne **jamais** force-push sur `main`.

---

## Issues & tickets

### Créer une issue

Utiliser les labels suivants :

| Label      | Usage                              |
|------------|------------------------------------|
| `bug`      | Comportement incorrect             |
| `feat`     | Nouvelle fonctionnalité à ajouter  |
| `refactor` | Amélioration technique             |
| `question` | Discussion ou point à clarifier    |
| `blocked`  | En attente d'une autre issue/PR    |

### Format recommandé

```
**Contexte** : Décris brièvement le problème ou le besoin.
**Comportement actuel** (si bug) : Ce qui se passe.
**Comportement attendu** : Ce qui devrait se passer.
**Étapes pour reproduire** (si bug) : Commandes, requêtes, config...
```

### Bonnes pratiques

- Une issue = **un seul sujet**.
- Mentionner l'issue dans le commit ou la PR avec `closes #<numéro>` pour la fermer automatiquement.
- Assigner l'issue à la personne qui la prend en charge.

---

*Ce document peut évoluer au fil du projet. Toute modification doit passer par une PR et être approuvée par l'équipe.*

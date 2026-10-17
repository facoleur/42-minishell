# Revue de code — 42-minishell

Revue effectuée le 2026-10-16 sur `main` (commit `5a4b589`), 29 fichiers `.c`
et 9 `.h` dans `src/` (3035 lignes), plus `libft/`.

**Méthode** : lecture intégrale de `src/`, compilation GCC 15 (`-Wall -Wextra
-Werror`), exécution sous AddressSanitizer + UndefinedBehaviorSanitizer +
LeakSanitizer, batterie de ~60 entrées de test comparées à `bash`, et
vérifications mécaniques de la Norme (80 colonnes, 25 lignes/fonction,
5 fonctions/fichier, tabulations, accolades, `for`/ternaires).

**Verdict court** : l'architecture est saine et bien découpée, la Norme est
respectée partout sauf dans le fichier en cours de modification. Mais **le
projet ne compile pas avec `-Werror`** (donc ne passe pas la correction en
l'état), il y a **un dépassement de tampon**, **une fuite de descripteurs de
fichiers qui finit par tuer le shell**, **~10 Ko de fuites mémoire sur une
session de 7 commandes** et une **erreur de conception dans l'ordre
tokenisation / expansion** qui permet à une variable d'environnement de créer
une redirection.

---

## 1. Bloquants

### 1.1 Le projet ne compile pas avec les flags obligatoires

```
$ make re
src/node_utils.c:92:9: error: 'free' called on unallocated object 'arg'
   92 |         free(arg);
      |         ^~~~~~~~~
src/node_utils.c:78:62: note: declared here
   78 | void    lst_append(t_node **lst, int type, char **cmd, char *arg[2])
cc1: all warnings being treated as errors
make: *** [Makefile:53: obj/node_utils.o] Error 1
```

`src/node_utils.c:78` déclare le paramètre comme `char *arg[2]`. GCC ≥ 12
interprète la taille explicite comme la promesse d'un tableau de taille connue
et refuse le `free()` de la ligne 92. Le pointeur vient bien de `palloc()`
(`merge_str`, `node_utils.c:70`) donc le `free` est correct — c'est la
signature qui est fausse.

**Correctif** : déclarer `char **arg` dans `src/node_utils.c:78` et dans
`src/node.h:37-38`.

C'est le premier bloquant pour une correction : sans ce changement, `make`
échoue sur une machine Linux récente.

### 1.2 Dépassement de tampon en lecture dans l'expansion

```
$ echo 'echo ${}' | ./minishell
AddressSanitizer: heap-buffer-overflow
    #0 handle_others src/expansion.c:36
    #1 expand_token src/expansion.c:84
    #2 expand_env_vars src/expansion_utils.c:25
```

Reproduit aussi par `echo $""` et `echo "$"`.

Cause (`src/expansion.c:31-41`) : `handle_others` sort de sa boucle avec
`k == strlen(token)`, puis fait `res[*j] = ft_substr(token, *i, k - *i + 1)`
— la longueur inclut le `\0` — et surtout `*i = k + 1`, soit **un cran
au-delà du terminateur**. La boucle appelante (`expansion.c:76`) relit alors
`token[i[0]]` hors du tampon.

**Correctif** : borner `k` à la longueur réelle et poser `*i = k` quand
`token[k] == '\0'`.

### 1.3 Fuite de descripteurs de fichiers : le shell meurt après ~500 lignes vides

`src/main.c:71-90` :

```c
while (ft_readline(&prompt))
{
    fd.in = dup(STDIN_FILENO);
    fd.out = dup(STDOUT_FILENO);
    if (ft_strlen(prompt) == 0 || is_quotes_opened(prompt))
        continue ;          /* <-- saute setback_fd() ET free(prompt) */
    ...
    setback_fd(&fd);
    free(prompt);
```

Le `continue` court-circuite la fermeture des deux `dup()` et le `free(prompt)`.
Mesure sur le binaire :

```
50 lignes vides -> 103 descripteurs ouverts
```

Soit 2 fd fuités par ligne vide ou par quote non fermée. À la limite `RLIMIT_NOFILE`
(1024 par défaut) le shell ne peut plus ouvrir ni pipe ni fichier. Un correcteur
qui appuie sur Entrée un moment, ou qui lance un tester automatisé, tombe dessus.

**Correctif** : fermer `fd.in`/`fd.out` et libérer `prompt` avant le `continue`
(ou déplacer les `dup()` après le test de ligne vide).

### 1.4 L'expansion a lieu avant la tokenisation des opérateurs

`src/tokenizer_utils2.c:40-62` enchaîne : normalisation → découpage sur les
espaces → **`expand_env_vars`** → `op_tokenizer` → `quotes_tokenizer`.

Comme l'expansion précède la reconnaissance des opérateurs, le contenu d'une
variable est réinterprété comme syntaxe :

```
$ export A=">"
$ echo hi $A zzz_created
            # crée le fichier zzz_created contenant "hi"
```

`bash` affiche `hi > zzz_created`. C'est le bug de parsing le plus visible et
il est systématiquement testé en correction.

**Correctif** : déplacer `expand_env_vars` après `op_tokenizer` /
`quotes_tokenizer`, sur les tokens finaux, en sautant ceux marqués
`SINGLE_QUOTE`.

### 1.5 Fuites mémoire

Session de 7 commandes sous LeakSanitizer : **10 121 octets en 218 allocations**.
Les sites principaux, par ordre d'impact :

| Site                                           | Nature                                                                                                                                                                                            |
| ---------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `src/env_utils.c:70` (`export_var`)            | `copy_env` duplique tout l'environnement et `*env = new_env` **perd l'ancien tableau et toutes ses chaînes**. Une fuite complète de l'environnement à chaque `export` d'une variable inexistante. |
| `src/expansion_utils.c:28` (`expand_env_vars`) | `tokens[i] = ft_strdup(tmp)` écrase le pointeur sans libérer le token d'origine. Double faute : le `ft_strdup` est superflu, `tmp` suffisait.                                                     |
| `src/builtin_utils.c:53` (`exe_builtin`)       | `printf("%s\n", builtin_pwd())` — le `getcwd()` n'est jamais libéré. Fuite à chaque `pwd`. Et si `getcwd` échoue, `printf("%s", NULL)`.                                                           |
| `src/builtin.c:116` (`builtin_echo`)           | `ft_putstr_fd(ft_strjoin(" ", cmd[i++]), 1)` — une fuite par argument de `echo`.                                                                                                                  |
| `src/history2.c:15-30` (`print_hist`)          | chaque ligne de `get_next_line` fuit, et le descripteur n'est jamais fermé (ni testé < 0).                                                                                                        |
| `src/main.c:104` (`copy_env`)                  | `env_copy` n'est jamais libéré ; `exit()` depuis `builtin_exit` ne libère rien non plus.                                                                                                          |
| `src/free.c:20` (`free_2starchar`)             | `if (!strstr                                                                                                                                                                                      |     | !\*strstr) return (NULL);` — un tableau **vide mais alloué** (`strstr[0] == NULL`) n'est jamais libéré. Cas atteint par `op_tokenizer` sur un token vide. |

---

## 2. Bugs de comportement

### 2.1 `stat()` non vérifié, lecture de structure non initialisée

`src/exec.c:15-41` :

```c
struct stat	path_stat;

stat(node->cmd[0], &path_stat);          /* retour ignoré */
if (access(node->cmd[0], F_OK)) ...
if (S_ISDIR(path_stat.st_mode))          /* lu même si stat a échoué */
```

Si `stat` échoue, `path_stat` reste non initialisé et `S_ISDIR` lit de la pile
non déterministe. L'ordre est aussi inversé : il faut tester l'existence
_avant_ de lire la structure.

### 2.2 Statut de sortie faux après un signal

`src/exec.c:72-83` ne traite que `WIFEXITED` :

```c
if (WIFEXITED(*status))
    *status = WEXITSTATUS(*status);
```

Un processus tué par un signal laisse le statut _brut_ de `wait`. Vérifié :

```
$ sleep 5        (Ctrl-C)
$ echo $?
0                # bash: 130
```

**Correctif** : ajouter `else if (WIFSIGNALED(*status)) *status = 128 + WTERMSIG(*status);`.

### 2.3 Messages d'erreur sans le nom de la commande

```
$ nosuchcmd
 command not found              # bash: bash: nosuchcmd: command not found
$ ./binaire_absent
No such file or directory       # bash: bash: ./binaire_absent: No such file or directory
$ cd /nope
 cd: No such file or directory  # bash: bash: cd: /nope: No such file or directory
```

Les messages commencent par une espace et ne portent ni le préfixe `minishell:`
ni le nom de la commande fautive. Dans le cas de « command not found », c'est
structurel : `get_cmd_path` (`src/cmd_utils.c:37`) **détruit `node->cmd`** avant
que `check_executable` ne produise le message, donc le nom n'est plus
disponible. Il faut conserver le nom avant de libérer.

Par ailleurs `perror(strerror(errno))` (`src/file_redirection.c:21,27,47,65`)
imprime le message d'erreur **deux fois** (`strerror` comme préfixe de
`perror`) : `No such file or directory: No such file or directory`.

### 2.4 Pas de découpage en mots après expansion

```
$ export A="-n hello"
$ echo $A ; echo FIN
-n hello
FIN
```

`bash` affiche `helloFIN` : il découpe l'expansion en deux mots et `-n` devient
l'option de `echo`. Le projet traite le résultat comme un seul argument.
C'est un test classique de correction.

### 2.5 Heredoc : pas d'expansion du corps

```
$ cat << EOF
> ligne1 $HOME
> EOF
ligne1 $HOME        # bash: ligne1 /home/facoleur
```

`bash` développe les variables dans le corps d'un heredoc **sauf** si le
délimiteur est entre quotes. Ici aucune expansion n'est faite. Le délimiteur
quoté est bien géré (`around_quotes_remover`), donc il suffit d'ajouter
l'expansion dans la branche non quotée.

Le heredoc est de plus lu au moment de l'exécution, dans un processus fils
(`src/file_redirection.c:97-123`), alors que `bash` collecte tous les heredocs
avant de lancer la commande. Visible sur `cmd << A | cmd2`.

### 2.6 Limitations artificielles

| Limite                                  | Lieu                | `bash`                                        |
| --------------------------------------- | ------------------- | --------------------------------------------- |
| `too much pipe` au-delà de 100 pipes    | `src/exec.c:96`     | pas de limite                                 |
| `too much heredoc` au-delà de 1 heredoc | `src/error.c:30-36` | plusieurs heredocs acceptés (`cat << A << B`) |

### 2.7 Terminal laissé modifié à la sortie

`set_signal` (`src/utils.c:34-36`) désactive `ECHOCTL` via `tcsetattr` à chaque
tour de boucle, et rien ne restaure les `termios` d'origine à la sortie. Après
un `exit`, le shell parent n'affiche plus `^C`. Il faut sauvegarder les
attributs au démarrage et les remettre avant de quitter.

### 2.8 Divergences mineures sur les builtins

- `echo` sans argument retourne **1** (`src/builtin.c:106`) ; `bash` retourne 0.
- `exit` sans argument sort avec 0 ; `bash` sort avec le statut de la dernière
  commande. `exit` n'écrit pas non plus `exit` sur stderr.
- `main` retourne toujours `0` (`src/main.c:110`) : sur Ctrl-D, le statut de la
  dernière commande est perdu.
- `export` n'est pas trié alphabétiquement ; `bash` trie `declare -x`.
- `export A` (sans `=`) crée une entrée sans `=` qui apparaît ensuite dans
  `env` ; `bash` ne la montre que dans `export`.
- `cd` ne met pas à jour `PWD` / `OLDPWD` et ne gère pas `cd -`.
- `unset` écrit son erreur sur **stdout** (`printf`, `src/env.c:70`) au lieu de
  stderr, et ne remonte aucun statut (retour `void`).
- `builtin_unset` ne libère pas l'entrée retirée dans tous les chemins
  (`src/env.c:64-84` décale les pointeurs après le `free`, correct, mais sans
  vérifier `does_var_exist` sur un nom invalide avant le message).

### 2.9 Fragilités de parsing

- `is_quotes_opened` (`src/tokenizer_utils.c:61-69`) est à la fois un
  **prédicat et un afficheur d'erreur**, et il est appelé deux fois par tour
  (`main.c:75` puis `tokenizer_utils2.c:46`). Le message n'apparaît qu'une fois
  grâce au `continue`, mais le couplage est dangereux. L'heuristique
  `count_quotes(string) == 1` est par ailleurs approximative.
- `around_quotes_remover` (`src/utils.c:52-70`) écrit `res[i - 2] = 0`. Pour une
  chaîne d'un seul caractère, c'est `res[-1]` — écriture hors tampon. Le cas est
  aujourd'hui filtré en amont par `is_quotes_opened`, mais rien ne le garantit.
- `normalize_loop` (`src/tokenizer_utils2.c:64-89`) lit `flag` (ligne 78) alors
  qu'il n'est **jamais initialisé**. Le flux actuel garantit qu'il est écrit
  avant d'être lu, mais c'est un UB latent à une refactorisation près.
- `palloc` (`libft/ft_calloc.c:15`) **ne met pas la mémoire à zéro** malgré son
  usage généralisé comme `calloc`, et appelle `exit(1)` en cas d'échec — ce qui
  rend impossible toute gestion d'erreur propre et fait sortir le shell sans
  libérer ni restaurer le terminal. Elle alloue aussi `elem_size * (size + 1)`,
  ce qui masque les `palloc(n)` où le `+1` du terminateur a été oublié.

---

## 3. Norme

### 3.1 Résultat des vérifications mécaniques

| Règle                                        | Résultat                                   |
| -------------------------------------------- | ------------------------------------------ |
| ≤ 80 colonnes (tab = 4)                      | ✅ `src/` ; ❌ `libft/ft_strdup.c:15` (81) |
| ≤ 25 lignes par fonction                     | ✅                                         |
| ≤ 5 fonctions par fichier                    | ✅                                         |
| Pas de `for`, `switch`, `do…while`, ternaire | ✅                                         |
| Pas d'affectation en déclaration             | ✅                                         |
| ≤ 4 paramètres                               | ✅                                         |
| Indentation par tabulations                  | ❌ `src/tokenizer_utils.c`                 |
| Accolades style Allman                       | ❌ `src/tokenizer_utils.c`                 |

### 3.2 `src/tokenizer_utils.c` est hors norme

C'est le seul fichier modifié dans l'arbre de travail, et il a été reformaté par
un `clang-format` au style LLVM par défaut :

```c
int is_single_op(char *string, int i) {
  char *single_op;
```

Indentation de 2 espaces, accolade en fin de ligne, pas de tabulation après le
type de retour. **8 occurrences** d'accolade en fin de ligne. C'est précisément
ce que le `.clang-format` livré avec cette revue corrige (section 5).

### 3.3 Code mort

Prototypes déclarés mais **jamais définis** — le lien ne casse que parce que
personne ne les appelle :

- `print_list` (`src/minishell.h:50`)
- `var_expander` (`src/minishell.h:52`)
- `debug_print` (`src/minishell.h:118`)
- `check_pipe_status` (`src/minishell.h:133`)
- `access_infile`, `print_str_array`, `print_tokens` — définis mais jamais appelés
- `set_is_in_quotes` (`src/tokenizer.c:88`) — définie, jamais appelée
- `word_counter_quotes` (`src/parser_utils.c:63`) — définie, jamais appelée

Déclarations dupliquées dans `src/minishell.h` : `is_single_op`, `is_double_op`,
`count_op` (lignes 76-78 **et** 84-86), `expand_env_vars` (lignes 53 **et** 54).

Types et macros jamais utilisés : `t_hist` (`history.h`), `t_nbr`
(`expansion.h`), le champ `is_in_quotes` et `unclosed_quote` de `t_tokens`,
`MINI_DEBUG_MSG`, l'intégralité de `colors.h` (8 macros), et dans `parser.h` :
`SINGLE`, `DOUBLE`, `NONE`, `QUOTED`, `NO_QUOTES`, `S_QUOTES`, `D_QUOTES`,
`CLOSED_QUOTES`, `OPEN_QUOTES`. Dans `error.h` : `ERR_GENERAL`,
`ERR_MISUSE_BUILTIN`, `ERR_SYNTAX`.

À supprimer : un correcteur lit les headers, et `colors.h` + 9 macros mortes
donnent l'impression d'un header copié sans être utilisé.

### 3.4 Fonctions interdites

- **`atoi`** (`src/builtin.c:90`) n'est pas dans la liste autorisée du sujet.
  `ft_atoi` existe déjà dans `libft`. C'est un motif de zéro.
- Coquille à corriger : `errror_msg` (trois `r`), `src/minishell.h:94` et
  `src/error_handling.c:15`.

---

## 4. Build et organisation

### 4.1 `Makefile`

1. **Aucune dépendance aux headers** : les règles `obj/%.o: src/%.c` ignorent
   `src/*.h`. Modifier `minishell.h` ne recompile rien → objets incohérents
   silencieusement. Correctif : ajouter `$(wildcard $(SRCDIR)/*.h)` en
   prérequis, ou générer les dépendances avec `-MMD -MP`.
2. **Pas de `.PHONY`** : `all`, `clean`, `fclean`, `re`, `run` cassent si un
   fichier homonyme existe. Le `Makefile` de `libft` le fait, lui.
3. `clean` fait `rm -f $(OBJS)` mais laisse le répertoire `obj/`, et
   **n'appelle pas `make -C libft clean`** — `fclean` contourne en faisant
   `rm -rf libft/*.o`, ce qui rate `libft/printf/*.o` et `libft/gnl/*.o`.
   Mieux : déléguer à `make -C $(LIBFTPATH) fclean`.
4. Pas de `-I$(SRCDIR)`, d'où les `#include "../libft/libft.h"` en chemin
   relatif dans `src/minishell.h:16`.
5. Les `$(CFLAGS)` ne sont pas passés à l'édition de liens, ce qui empêche un
   `make CFLAGS=-fsanitize=address` de fonctionner (lien cassé, vérifié).
6. `-lreadline` sans `-lncurses`/`-ltinfo` : échoue sur certaines distributions
   où readline n'embarque pas termcap.

### 4.2 `.gitignore`

`minishell` (le binaire) et `obj/` ne sont pas ignorés — `git status` les
affiche comme non suivis. Ajouter :

```
minishell
obj/
```

### 4.3 CI (`.github/workflows/c-cpp.yml`)

- Tourne sur `macos-latest` uniquement : **la correction se fait sur Linux**, et
  c'est précisément sur Linux que `make re` échoue (§1.1). La CI est donc verte
  sur un échec de correction.
- `./tester || true` : le résultat du tester ne peut jamais faire échouer le
  job. La CI ne vérifie en pratique que la compilation macOS.
- Correctif : matrice `ubuntu-latest` + `macos-latest`, retirer le `|| true`, et
  ajouter une étape `norminette`.

### 4.4 Headers

- `src/expansion.h:16` inclut `minishell.h`, qui inclut `expansion.h`
  (`minishell.h:19`) : **inclusion circulaire**, fonctionnelle grâce aux gardes
  mais à casser.
- `src/minishell.h` n'inclut pas `env.h`. `src/env_utils.c` définit
  `does_var_exist`, `export_var`, `is_varname_valid`, `is_varvalue_valid` **sans
  que leurs prototypes soient visibles** dans l'unité de compilation (seul
  `env.c` inclut `env.h`). Ça compile parce que définition et appels sont dans
  le même fichier, mais toute divergence de signature passerait inaperçue.
- `src/minishell.h` mélange 70 prototypes de 20 modules avec des commentaires
  de section désynchronisés (`// var_expansion.c` pour des fonctions qui sont
  dans `expansion_utils.c`, `// erreur.c` pour `error.c`, `// infile.c` pour
  `file_redirection.c`). Les lignes 166-167 contiennent deux fois la même macro
  de debug commentée.

### 4.5 `README.md`

La to-do list est obsolète : `>`, `>>`, `<<`, l'expansion de variables, `$?`,
`echo -n`, `export`, `unset`, `env`, `exit`, Ctrl-C/D/\\ sont tous **implémentés
et cochés `[ ]`**. Un correcteur qui lit le README croit le projet inachevé.

---

## 5. Ce qui est bien

Pour ne pas donner une image fausse : le projet est globalement bien construit.

- **Découpage clair** : tokenizer / parser / exec / builtins / redirections /
  pipes sont des modules distincts, chacun ≤ 135 lignes, ≤ 5 fonctions.
  C'est rare et ça rend la revue possible.
- **Norme respectée** dans 28 fichiers sur 29, y compris les contraintes dures
  (25 lignes, 5 fonctions, pas de `for`).
- **La liste doublement chaînée `t_node`** avec `previous` est le bon choix pour
  remonter aux redirections d'un segment de pipeline — `run_redirection_file`
  et `set_pipe` en tirent un code court et lisible.
- **Les pipes fonctionnent**, builtins compris (`export | head -2`, `pwd | cat`
  vérifiés), y compris les chaînes de 4 commandes.
- **Les redirections se combinent correctement** dans les deux ordres
  (`cat < in > out` et `cat > out < in` vérifiés).
- **`$?` est juste** pour les sorties normales : 127 (introuvable), 126, 1, 2
  propagés correctement.
- **Les quotes sont gérées finement** : `is_in_quotes` suit l'état
  simple/double caractère par caractère, et `echo 'single $HOME'` vs
  `echo "$HOME"` se comportent comme dans `bash`.
- **Aucun crash** sur les 60 entrées malformées testées (`|`, `>`, `|||`,
  `><`, `""`, `$`, quotes non fermées…) hors les trois cas du §1.2.
- **`SHLVL` est incrémenté** au démarrage — détail que beaucoup de minishell
  oublient.

---

## 6. `.clang-format` livré

Un fichier `.clang-format` est ajouté à la racine. Il encode les règles de la
Norme que `clang-format` sait exprimer :

- tabulations de 4, pour l'indentation **et** l'alignement (`UseTab: Always`) ;
- 80 colonnes ;
- accolades Allman ;
- `if (x)` avec espace, `f(x)` sans (`SpaceBeforeParens: ControlStatements`) ;
- `char *str` / `char **env` (`PointerAlignment: Right`) ;
- aucune forme condensée sur une ligne ;
- continuations de condition indentées par tabulation, opérateur en tête de
  ligne (`AlignOperands: DontAlign`, `BreakBeforeBinaryOperators: NonAssignment`) ;
- `SortIncludes: false` — les includes de la Norme sont ordonnés à la main.

**Validation faite** : appliqué aux 37 fichiers de `src/`, le résultat
**compile et s'exécute** à l'identique, et passe toutes les vérifications
mécaniques de la Norme (80 colonnes, 25 lignes, tabulations, Allman). Appliqué
à `src/tokenizer_utils.c`, il restaure les tabulations et les accolades Allman.

### Les trois règles que `clang-format` ne sait pas exprimer

| Attendu par la Norme                                   | Produit par `clang-format` |
| ------------------------------------------------------ | -------------------------- |
| `int<TAB>main(void)`                                   | `int main(void)`           |
| `char<TAB><TAB>*prompt;` (nom au taquet de tabulation) | `char *prompt;`            |
| `# define FOO_H` dans un include guard                 | `#define FOO_H`            |

Ces trois points sont hors du modèle de `clang-format` : il n'a aucune option
pour insérer une tabulation entre le type de retour et le nom, pour aligner sur
un taquet de tabulation plutôt que sur la colonne suivante, ni pour indenter
dans un include guard (qu'il exclut volontairement).

**Ils sont exactement ce que `c_formatter_42` post-traite** — l'outil déjà
référencé dans `libft/Makefile` (cible `norme`). Le `.clang-format` livré est
pensé pour se composer avec lui :

```sh
pip install c-formatter-42
c_formatter_42 src/*.c src/*.h     # clang-format + les 3 corrections
```

Sans `c_formatter_42`, `clang-format --style=file -i src/*.c src/*.h` fait 95 %
du travail et laisse ces trois points à retoucher.

Deux cibles à ajouter au `Makefile`, sur le modèle de celui de `libft` :

```make
norme:
	c_formatter_42 $(addprefix $(SRCDIR)/,$(SRC)) $(SRCDIR)/*.h

normi:
	norminette $(SRCDIR) $(LIBFTPATH)
```

---

## 7. Ordre de traitement suggéré

**À faire avant toute correction** (sinon c'est un échec immédiat) :

1. `char *arg[2]` → `char **arg` — débloque `make re` (§1.1)
2. `atoi` → `ft_atoi` — fonction interdite (§3.4)
3. Reformater `src/tokenizer_utils.c` (§3.2) — `clang-format` le fait
4. Fermer les fd et libérer `prompt` avant le `continue` de `main.c` (§1.3)
5. Dépassement de tampon de `handle_others` (§1.2)

**Puis, par rapport qualité/effort décroissant :**

6. Déplacer l'expansion après la tokenisation des opérateurs (§1.4) — le bug
   de parsing le plus visible
7. `WIFSIGNALED` → `128 + WTERMSIG` (§2.2) — 2 lignes, teste à coup sûr
8. Les 7 sites de fuite du §1.5, en commençant par `export_var` et
   `expand_env_vars`
9. Messages d'erreur avec le nom de la commande, et le double `strerror` dans
   `perror` (§2.3)
10. `stat()` non vérifié (§2.1)
11. Expansion dans le corps des heredocs (§2.5)
12. Découpage en mots après expansion (§2.4)
13. Retirer le code mort et `colors.h` (§3.3), mettre le README à jour (§4.5)
14. Dépendances aux headers et `.PHONY` dans le `Makefile` (§4.1), CI sur
    Linux (§4.3)

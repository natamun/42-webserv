# Contributing

## Branches
- `main` est la branche stable, jamais de push direct
- Nommage : `feat/nom`, `fix/nom`, `ci/nom`, `refactor/nom`

## Commits
Format : `type: description courte en anglais`

| Type | Quand |
|------|-------|
| `feat:` | nouvelle fonctionnalité |
| `fix:` | correction de bug |
| `ci:` | pipeline, GitHub Actions |
| `refactor:` | réorganisation sans changer le comportement |
| `docs:` | README, commentaires |
| `test:` | scripts de test |
| `chore:` | Makefile, .gitignore, config |

Exemples :
- `feat: add user authentication`
- `fix: handle null pointer on early exit`
- `refactor: extract parsing logic into separate class`
- `docs: update README with installation steps`

## Pull Requests
- Une PR = une seule fonctionnalité ou fix
- La pipeline doit être verte avant de merger
- Le binôme doit approuver avant de merger
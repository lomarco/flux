# Commit Message Guidelines

This repository adheres to the [Conventional Commits](https://www.conventionalcommits.org/) specification.

## Commit Message Format
Each commit message should conform to the following structure:
`<type>[optional scope]: <short description>...`

- `<type>`: Designates the commit type (mandatory)
- `[optional scope]`: Specifies the code area of functionality impacted (optional)
- `<short description>`: Provides a succinct summary of the changes (mandatory)
- Detailed description and references: optional

## Commit Types
| Type       | Description                                                | Example                                    |
|------------|------------------------------------------------------------|--------------------------------------------|
| `feat`     | Introduces a new feature.                                  | `feat(lex): Add lexer`                     |
| `fix`      | Rectifies a bug.                                           | `fix(cart): Correct discount calculation`  |
| `chore`    | Maintenance tasks and build updates.                       | `chore: Add .clangd file`                  |
| `docs`     | Documentation changes.                                     | `docs: Update README`                      |
| `refactor` | Code changes that neither fix a bug nor add a feature.     | `refactor(api): Improve request structure` |
| `style`    | Code style changes (formatting, missing semicolons, etc.). | `style: Fix indentation`                   |
| `merge`    | Denotes a merge commit.                                    | `merge: Merge dev to master`               |
| `test`     | Adding or fixing tests.                                    | `test(user): Add registration tests`       |
| `init`     | Signifies the initial commit.                              | `init: Initial commit`                     |

## Additional Recommendations
- Use imperative mood in the short description (e.g., "add", "fix").
- Limit the first line to 50 characters.
- Separate the subject from the body with a blank line.
- Include issue or bug references in the body if needed.

# Commit Message Guidelines

In this project, i follow the [Conventional Commits](https://www.conventionalcommits.org/) standard to maintain a clear and structured history of changes.

## Commit Message Format

A commit message should have the following format:

- `<type>` — commit type (required)  
- `[optional scope]` — the area of the code or functionality affected (optional)  
- `<short description>` — concise summary of the change (required)  
- Detailed description and references — optional

## Commit Types

| Type       | Description                                               | Example                                    |
|------------|-----------------------------------------------------------|--------------------------------------------|
| `feat`     | A new feature                                             | `feat(lex): Add lexer`                    |
| `fix`      | A bug fix                                                 | `fix(cart): Correct discount calculation`  |
| `docs`     | Documentation changes                                     | `docs: Update README`                      |
| `style`    | Code style changes (formatting, missing semicolons, etc.) | `style: Fix indentation`                   |
| `refactor` | Code changes that neither fix a bug nor add a feature     | `refactor(api): Improve request structure` |
| `test`     | Adding or fixing tests                                    | `test(user): Add registration tests`       |
| `chore`    | Maintenance tasks, build process updates, etc.            | `chore: Update dependencies`               |
| `merge`    | Merge commit                                              | `merge: Merge dev to master`               |

## Additional Recommendations

- Use imperative mood in the short description (e.g., "add", "fix").  
- Limit the first line to 50 characters.  
- Separate the subject from the body with a blank line.  
- Include issue or bug references in the body if needed.

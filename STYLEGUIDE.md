# Style guide

## Code Style
This project follows the [Chromium C/C++ coding style](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/styleguide/styleguide.md)

## C Standatd
Flux is developed following the GNU C 17 standard.

## Commit Style
This repository adheres to the [Conventional Commits](https://www.conventionalcommits.org/) specification.

### Commit Message Format
Each commit message should conform to the following structure:
`<type>[optional scope]: <short description>...`
- `<type>`: Designates the commit type (mandatory)
- `[optional scope]`: Specifies the code area of functionality impacted (optional)
- `<short description>`: Provides a succinct summary of the changes (mandatory)
- Detailed description and references: optional

### Commit Types
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

### Additional Tips
- Use imperative mood in the short description (e.g., "add", "fix").
- Limit the first line to 50 characters.
- Separate the subject from the body with a blank line.
- Include issue or bug references in the body if needed.

## Git Branching Style and Methodology
This project follows the (Trunk-Based Development (TBD) methodology)[https://trunkbaseddevelopment.com/], which emphasizes a single mainline branch (referred to as the trunk).

### Branching Style
- The master branch is the `trunk` branch and always contains stable, production-ready code.
- Developers create `short-lived` feature branches directly from the trunk for each new task or feature.
- Feature branches should be merged back into the trunk as soon as the changes are tested and stable.
- **Long-lived** branches are **discouraged** to minimize merge conflicts and integration issues.
- Branch names should be descriptive and concise, reflecting the purpose of the branch, for example:
    `feature/user-authentication`, `fix/payment-bug`, `chore/update-dependencies`.

### Branching Message Format
`<type>/<short-description>[-<optional-id>]`
- `<type>`: Defines the purpose or category of the branch (mandatory)
- `<short-description>`: A concise, lowercase summary of the work, using hyphens to separate words (mandatory)
- `[-<optional-detail>]`: Additional context such as ticket number, author initials, or environment (optional)

### Branches Types
| Type      | Description                     | Example                       |
|-----------|---------------------------------|-------------------------------|
| `feature` | For new features.               | `feature/add-lexer`           |
| `bugfix`  | For bug fixes.                  | `bugdix/correct-lexer`        |
| `hotfix`  | For ugent fixes on production.  | `hotfix/payment-timeout-0425` |
| `chore`   | For maintenance or refactoring. | `chore/code-cleanup`          |

### Additional Tips 
- Keep branch names short but descriptive enough to understand the purpose.
- Use lowercase letters and hyphens for readability.
- Avoid special characters or spaces.
- Since trunk-based development encourages short-lived branches, merge back to trunk frequently and delete branches after merging.

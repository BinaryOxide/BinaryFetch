# Contributing to BinaryFetch

Thank you for your interest in **BinaryFetch**.

BinaryFetch is an open-source project developed under the **BinaryFetch-CLI** organization, with a strong focus on architectural clarity, stability, and long-term maintainability.

Please read this document carefully before contributing.

---

## Development Model

BinaryFetch follows a **maintainer-driven, phase-based development model**.

This means:
- The source code is open and publicly available.
- Development priorities are defined by the core maintainers.
- Contributions are accepted **only if they align with the current development focus**.

This approach helps prevent architectural drift and ensures consistent, intentional growth during the project’s active building stage.

---

## Platform Development Status

BinaryFetch currently supports multiple platforms at **different stages of maturity**.

### Windows
- The Windows version of BinaryFetch has already been released.
- The core architecture for Windows is considered stable.

✅ **Accepted contributions for Windows:**
- Bug fixes
- Stability improvements
- Minor refinements
- Regression fixes

❌ **Not accepted for Windows (without approval):**
- Large refactors
- Major architectural changes
- Feature expansions outside the roadmap

---

### Linux
- The Linux version of BinaryFetch is still under **active construction**.
- Core architecture, module boundaries, and design patterns are still being established.

Because of this, Linux development follows a **strict phase-based model** to maintain architectural consistency.

---

## Why Phase-Based Contributions for Linux?

Allowing unrestricted or parallel feature development during Linux’s construction phase can:
- Introduce architectural inconsistency
- Increase technical debt
- Slow down long-term progress

By focusing on **one clearly defined phase at a time**, we ensure:
- Clean and predictable system design
- Easier review and testing
- Stronger foundations for future expansion

As the Linux implementation matures, contribution scope will gradually expand.

---

## Current Linux Development Phase

At any given time, Linux development focuses on **one specific phase**.

Examples:
- Phase 1: Linux CPU information
- Phase 2: Linux GPU information
- Phase 3: Linux Memory information

👉 **Only Linux contributions related to the currently active phase are accepted.**

The active phase is always documented in:
- The project README
- A pinned GitHub issue or milestone

---

## Contribution Rules (Important)

### ✅ Allowed
- Windows bug fixes and stability improvements
- Linux contributions related to the current phase
- Improvements or refactors within the approved scope
- Documentation updates
- Issues and discussions aligned with the roadmap

### ❌ Not Allowed
- Linux features unrelated to the current phase
- Random module additions
- Platform expansions outside the roadmap
- Large refactors not explicitly requested by maintainers

Well-written code may still be declined if it is **out of scope**.

---

## Issues Before Pull Requests

Pull requests **must be linked to an approved issue**.

### Workflow
1. Check the current platform focus (Windows or Linux)
2. Verify the active Linux development phase (if applicable)
3. Pick an existing issue  
   **or**
4. Open a new issue describing your idea
5. Wait for maintainer approval before starting work

Unlinked or unsolicited PRs may be closed without review.

---

## Pull Request Guidelines

- Target the appropriate branch as specified by maintainers
- Keep PRs focused and minimal
- Follow the existing coding style
- Avoid unrelated changes in the same PR
- Write clear and descriptive commit messages

Maintainers reserve the right to request changes or decline PRs.

---

## Forks and Attribution

Forking BinaryFetch is allowed under the project license.

However:
- The project name must not be misrepresented
- Original authorship and license information must remain intact
- Forks must not claim to be the original BinaryFetch project

---

## Future Contributions

As BinaryFetch matures:
- Linux development phases will broaden
- Contribution scope may expand
- Additional maintainers may be added

Current restrictions reflect the project’s **active construction phase** and may change over time.

---

## Final Note

BinaryFetch prioritizes **direction over volume**.

Clear goals, disciplined development, and architectural consistency matter more than rapid feature expansion.

Thank you for respecting the project’s vision.

— **BinaryFetch-CLI Maintainers**

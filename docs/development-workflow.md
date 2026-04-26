# Development Workflow

## Typical Daily Loop

1. Create a feature branch.
2. Build firmware.
3. Run host tests.
4. Flash and sanity check on hardware.
5. Commit with conventional commit message.

## Branch Naming

- feat/<topic>
- fix/<topic>
- refactor/<topic>
- doc/<topic>
- test/<topic>

## Commit Conventions

- feat: user-visible feature
- fix: bug fix
- refactor: non-behavioral code cleanup
- test: test additions/changes
- docs: documentation only
- chore: build/tools/config updates

## Quality Gate Before Push

- Firmware build passes
- Host tests pass
- No unexpected warnings introduced
- README/docs updated when behavior changes

## Suggested Next Documentation Tasks

1. Add architecture.md with app/driver boundaries.
2. Add fdcan.md with TX/RX implementation notes.
3. Add testing.md with test patterns and naming.
4. Add hardware-validation.md with smoke-test checklist.

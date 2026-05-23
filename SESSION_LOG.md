# C-Todo Session Log

**Date:** May 23, 2026  
**Project:** c-todo — Terminal Task Manager  
**Status:** MVP Complete, Ready for GUI Phase

---

## What We Achieved Today

### 1. **Planned & Designed the Project** (Phase 0)
- Identified requirements for a terminal-based todo app
- Chose SQLite for reliable, single-file storage
- Designed CLI interface with subcommands (`add`, `list`, `done`, `rm`, etc.)
- Planned 5 implementation phases with clear deliverables

### 2. **Built the Core Application** (Phases 1-4)
- **Foundation:** Makefile, SQLite schema, task struct, basic CRUD
- **Core CRUD:** Complete, undo, delete with color-coded output
- **Extended features:** Edit, search, clear, priority levels, due dates
- **Polish:** Overdue highlighting, strikethrough for completed tasks, empty states
- **Result:** Full-featured CLI todo app in ~720 lines of C17

### 3. **Testing & Quality**
- Wrote 17 comprehensive unit tests (7 DB, 10 CLI)
- All tests passing with zero warnings
- Coverage: database ops, CLI parsing, edge cases, error handling
- Fixed security issue: replaced `atoi()` with `strtol()` for safe integer parsing

### 4. **Security Audit**
- **SQL Injection:** ✅ Safe — all queries use parameterized bindings
- **Buffer Overflow:** ✅ Safe — all string ops use `snprintf` with `sizeof`
- **Memory Safety:** ✅ Safe — proper malloc/free patterns, null checks
- **Secrets:** ✅ None hardcoded, no network access
- **Result:** Production-ready security posture for a local CLI tool

### 5. **Documentation & Release**
- Wrote comprehensive README with usage examples
- Created Makefile targets: `make`, `make install`, `make uninstall`, `make test`
- Initialized git with clean commit history
- Created public GitHub repo: [tigerHo69/c-todo](https://github.com/tigerHo69/c-todo)
- Documented design philosophy, project structure, and SQLite choice

---

## Project Stats

| Metric | Value |
|--------|-------|
| **Source Code** | 8 files, 720 lines |
| **Tests** | 2 files, 17 tests, 322 lines |
| **Binary Size** | 52KB |
| **Build Time** | ~1 second |
| **Dependencies** | SQLite only |
| **Compiler** | C17, zero warnings |

---

## Features Implemented

### Commands
- `todo add "<desc>" [-p priority] [-d date]` — Add a task
- `todo list [--all|--done|--pending]` — List tasks with filters
- `todo done <id>` — Mark task complete
- `todo undo <id>` — Reopen a task
- `todo rm <id>` — Delete a task
- `todo edit <id> [desc] [-p pri] [-d date]` — Modify task fields
- `todo search <term>` — Find tasks by substring
- `todo clear` — Remove completed tasks
- `todo help` — Show usage

### Display Features
- Color-coded priorities (red=high, yellow=med, dim=low)
- Overdue dates highlighted in red
- Completed tasks dimmed with ✓ checkmark
- Smart sorting: by priority, then due date
- `NO_COLOR` env var support for terminals without color
- Custom database path via `TODO_DB` env var

### Data Model
```
Tasks:
  - id (auto-increment)
  - description (required)
  - priority (high/med/low, default: med)
  - due_date (optional, YYYY-MM-DD format)
  - created_at (auto)
  - completed (boolean)
  - completed_at (auto when marked done)
```

---

## Next Phase: GUI Implementation

### Why Add a GUI?
- CLI is powerful but intimidates non-technical users
- GUI provides visual task overview at a glance
- Drag-and-drop, color-coding, and visual feedback improve UX
- Can coexist with CLI — use either or both

### Recommended Approach: **Dear ImGui** (IMGUI)
**Why Dear ImGui?**
- Minimal C/C++ library (single .h + .cpp)
- Immediate-mode GUI (simple, no complex widget trees)
- Cross-platform: Windows, macOS, Linux
- No external dependencies beyond graphics (uses OpenGL or Vulkan)
- Compiles to ~2-3MB binary
- Perfect for developer tools & utility apps

### Architecture
```
c-todo/
├── src/
│   ├── cli.c / db.c / task.c  (keep unchanged)
│   ├── gui.c                   (new: Dear ImGui wrapper)
│   └── gui_main.c              (new: GUI entry point)
├── imgui/                       (bundled submodule or vendored)
└── Makefile                     (add gui target)
```

### Implementation Plan

#### Phase 1: GUI Foundation (4-6 hours)
- [ ] Bundle Dear ImGui source
- [ ] Create window with task list
- [ ] Render tasks with priorities, dates, checkmarks
- [ ] Reuse `task_print_*` logic for display

#### Phase 2: Basic Interactivity (4-5 hours)
- [ ] Click to complete/uncomplete task
- [ ] Click to delete task
- [ ] Modal dialog to add new task
- [ ] Real-time list updates from database

#### Phase 3: Full Feature Parity (4-6 hours)
- [ ] Edit task modal (description, priority, due date)
- [ ] Search/filter UI
- [ ] Color theming (light/dark mode)
- [ ] Drag-to-reorder (optional, nice-to-have)

#### Phase 4: Polish (2-3 hours)
- [ ] Smooth animations on state changes
- [ ] Keyboard shortcuts (Ctrl+N for new, etc.)
- [ ] Window persistence (size, position, sort order)
- [ ] Error dialogs for failures

**Total Estimate:** 14-20 hours

### Branching Strategy
```
main (stable, CLI-only)
  ├── gui-foundation (experimental)
  └── gui-complete (ready to merge)
```

Keep CLI on main, develop GUI on feature branch, merge when stable.

### Build Options
After GUI is ready, support both:
```bash
make              # CLI only (current)
make gui          # GUI only
make all          # Both (user chooses on launch)
```

---

## Key Decisions & Trade-offs

| Decision | Why | Trade-off |
|----------|-----|-----------|
| **SQLite** | Single file, reliable, no server | Slightly larger binary |
| **C17 only** | Fast, portable, no runtime | More verbose than scripting languages |
| **CLI-first** | Unix philosophy, powerful, simple | Requires terminal literacy |
| **Dear ImGui** | Lightweight, cross-platform, easy | Less polished than Qt/GTK |
| **Reuse DB layer** | DRY, consistent storage, tested | GUI tightly couples to C backend |

---

## Known Limitations & Future Ideas

### Limitations
- Single-user only (no multi-device sync)
- No tags or categories
- No recurring tasks
- Date format strict (YYYY-MM-DD)

### Future Ideas
- Cloud sync (Dropbox, iCloud sync folder)
- Tags / multi-label support
- Recurring tasks (daily, weekly, etc.)
- Undo history
- Dark theme variants
- Mobile app (separate codebase)
- Integration with calendar apps

---

## How to Continue

### For CLI-only users:
```bash
git clone https://github.com/tigerHo69/c-todo
cd c-todo
make
sudo make install
todo add "Get started"
```

### For GUI development:
```bash
git checkout -b gui-foundation
# Add Dear ImGui integration
# Build phase 1: window + task list rendering
# ...test, iterate, merge when stable
```

---

## Session Reflection

**What Went Well:**
- Fast iteration from design to working product
- Clean separation of concerns (cli, db, task, main)
- Comprehensive testing caught edge cases
- Zero security vulnerabilities in audit
- Good documentation from the start

**What Could Be Better:**
- More advanced sorting options (custom order, grouping)
- Task notes / descriptions longer than 256 chars
- Bulk operations (complete all, delete all overdue)
- Performance testing with 10k+ tasks

**Next Session Focus:**
- Start GUI foundation (window + basic rendering)
- Decide on graphics backend (OpenGL, Metal, Vulkan)
- Set up build system for GUI (CMake or extended Makefile)

---

**Status:** ✅ Ready for GUI phase. CLI is production-ready.  
**Repository:** https://github.com/tigerHo69/c-todo  
**Commits:** 2 (initial + docs update)  
**Next Meeting:** GUI architecture & Dear ImGui integration

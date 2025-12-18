

# ASCII Art Generator Engine — Developer Documentation

## Purpose

The ASCII Art Generator Engine is responsible for **ensuring the presence of user ASCII art** in the proper directory and **copying a default bundled version** when needed. This allows BinaryFetch to function reliably on first run, supports user customization, and separates visual assets from program logic.

---

## Target User File

```
C:\Users\<USERNAME>\AppData\BinaryFetch\AsciiArt.txt
```

* All runtime operations interact with this path.
* The engine **never modifies the bundled default**, only the user copy.
* Bundled default: `Default_Ascii_Art.txt` is **inside the source code / project assets**.

---

## Workflow: Detailed Step-by-Step

### Step 0 — Initialization

* `main.cpp` prepares `AsciiArt` loader object.
* Sets path for:

  * User ASCII: `%LOCALAPPDATA%\BinaryFetch\AsciiArt.txt`
  * Bundled default: `<EXE_DIR>/Default_Ascii_Art.txt`

---

### Step 1 — Main requests ASCII content

```text
main -> ASCII Engine: load user ASCII at path
```

* `main` calls engine to check/load `AsciiArt.txt`.
* Engine attempts to **open the user file**.

---

### Step 2 — Engine reports missing file

```text
ASCII Engine -> main: file missing
```

* Engine returns a **status** (e.g., `FileNotFound` or `Missing`) to `main`.
* Engine does **not** create the file automatically at this stage.
* `main` now knows that the ASCII asset is absent.

---

### Step 3 — Main commands engine to create file

```text
main -> ASCII Engine: "Create AsciiArt.txt from default"
```

* `main` explicitly instructs the engine to generate the missing file.
* This maintains **centralized control** and allows optional logging or user notification in `main`.

---

### Step 4 — Engine generates the file

1. Engine ensures parent directory exists (`BinaryFetch` in AppData).
2. Creates `AsciiArt.txt` in that directory.
3. Opens the **bundled `Default_Ascii_Art.txt`** for reading.
4. Copies content from default into `AsciiArt.txt`:

   * Use atomic write: write to temp file → flush → rename to `AsciiArt.txt`.
5. Returns **status** to `main`:

   * `CreatedFromDefault` on success
   * `IoError` / `PermissionDenied` on failure

---

### Step 5 — Main re-attempts loading

* `main` now tries to load the newly created `AsciiArt.txt`.
* On success → normal rendering begins.
* On failure → fallback or error message (optional, in-memory default or console log).

---

### Step 6 — Summary diagram

```text
main -> ASCII Engine : load user ASCII
ASCII Engine -> main  : file missing
main -> ASCII Engine  : create from default
ASCII Engine -> main  : CreatedFromDefault
main -> ASCII Engine  : load user ASCII again
ASCII Engine -> main  : Success -> continue normal execution
```

---

## Developer API — Recommended

### Status Enum

```cpp
enum class AsciiGeneratorStatus {
    Success,               // File exists or successfully created
    FileAlreadyExists,     // User file already present
    CreatedFromDefault,    // Created new user file from bundled default
    MissingBundledDefault, // Default_Ascii_Art.txt missing in source/bundle
    IoError,               // Generic I/O error
    PermissionDenied,      // Cannot write to target location
};
```

### Main function interface

```cpp
// Check or create user ASCII art
AsciiGeneratorStatus ensureUserAsciiExists(
    const std::filesystem::path& userPath,
    const std::filesystem::path& bundledDefaultPath);
```

* **Input:** user path + bundled default path
* **Output:** status code
* **Behavior:** creates file **only when instructed** by `main`.

---

## Implementation Notes

* **Idempotence:** repeated calls must not overwrite existing user files.
* **Atomic writes:** temp file + rename to avoid partial file corruption.
* **Error handling:** engine returns error codes; `main` decides how to notify the user.
* **Bundled default:** always UTF-8, no BOM, and included inside source for compilation.
* **Directory creation:** engine creates missing directories automatically.

---

## Testing / Edge Cases

1. **User file exists:** status `FileAlreadyExists`; content remains untouched.
2. **User file missing, default present:** status `CreatedFromDefault`; file matches bundled default.
3. **Bundled default missing:** status `MissingBundledDefault`; engine does not create user file.
4. **Permission denied:** engine returns `PermissionDenied`; `main` logs and skips creation.
5. **Concurrent runs:** temp files should be isolated; rename failures handled gracefully.


Perfect! Let’s update the documentation for the **role of `main`** specifically, incorporating your clarification. This will make it crystal clear for developers.

---

## Role of `main.cpp` in ASCII Art Workflow

The `main` function acts as the **coordinator** for ASCII art handling. It does **not** generate ASCII content itself but **delegates all file creation and loading tasks to the ASCII Art Generator Engine**. Its responsibilities include:

---

### 1. Request ASCII art from the engine

* `main` calls the ASCII engine (`AsciiArtControlEngine`) to **load the user ASCII art file**.
* Engine attempts to open `AsciiArt.txt` in the designated user folder.
* Engine returns **success** if the file exists and is readable.

---

### 2. Handle missing file

* If the engine reports that the file is missing:

  1. `main` instructs the engine to **generate `AsciiArt.txt`**.
  2. The engine creates the file in the proper folder.
  3. The engine copies content from the **bundled `Default_Ascii_Art.txt`** into the newly created user file.

---

### 3. Retry loading

* After generation, `main` requests the engine to **load the ASCII art again**.
* On success → normal rendering begins.
* On failure → fallback logic (optional, e.g., console warning or in-memory default).

---

### 4. Workflow diagram (main’s perspective)

```text
main.cpp
  |
  |-- request ASCII Art -> ASCII Engine
        |
        |-- file exists? --- yes --> continue normal execution
        |                   no
        |
        |-- instruct engine to generate -> engine creates AsciiArt.txt from Default_Ascii_Art.txt
        |
        |-- retry load ASCII Art -> success? ---> continue normal execution
```

---

### 5. Key Responsibilities

1. **Coordinator**: Detects missing user files and instructs engine to create them.
2. **Delegate**: Leaves all file creation and default content management to the engine.
3. **Fallback handling**: Decides what to do if engine cannot generate/load the art.
4. **Maintains first-run stability**: Ensures BinaryFetch always has valid ASCII content to render.

---

Alright, let’s break down the **role of `AsciiArtControlEngine.cpp`** in your project, fully detailed for developers. This will clarify what it does, what it **should** do, and how it interacts with `main.cpp` and the files.

---

## Role of `AsciiArtControlEngine.cpp`

The `AsciiArtControlEngine.cpp` is the **dedicated ASCII Art Generator Engine**. Its responsibility is to **ensure that valid ASCII art exists** for BinaryFetch without main needing to handle file creation or default content.

It **does not render** the ASCII art—that’s `LivePrinter`’s job. The engine is purely for **file management, generation, and content provisioning**.

---

### 1. Responsibilities

1. **Load ASCII art file**

   * Checks for the existence of `AsciiArt.txt` in the target folder (e.g., `C:\Users\<USERNAME>\AppData\BinaryFetch\`).
   * Reads the file line by line and stores it internally for the main program to use.

2. **Report missing file**

   * If the file does not exist or cannot be opened, the engine **informs main.cpp** that ASCII art is unavailable.
   * It does **not generate the default art automatically**—main triggers generation explicitly.

3. **Generate ASCII art file**

   * On instruction from main.cpp, the engine:

     1. Creates `AsciiArt.txt` in the target folder if it does not exist.
     2. Copies the content from the **bundled `Default_Ascii_Art.txt`** into the new file.
   * Ensures that the newly created file is **fully renderable**, **properly aligned**, and **terminal-safe**.

4. **Reload ASCII art**

   * After generation, the engine can reload the file to confirm successful creation.
   * Provides the loaded lines back to main.cpp or `LivePrinter` for rendering.

5. **Sanitization and validation**

   * Trims invisible characters (BOM, zero-width spaces) for safe rendering.
   * Precomputes line widths for correct alignment alongside system info.

---

### 2. Interaction with `main.cpp`

1. `main` requests ASCII art load → engine tries to open `AsciiArt.txt`.
2. Engine reports **success or missing file**.
3. If missing → main instructs the engine to **generate** the file from `Default_Ascii_Art.txt`.
4. Engine creates the file, copies default content, and reloads it.
5. Engine reports success → main proceeds to render the art with system info.

---

### 3. Execution Flow (Engine Perspective)

```text
ASCII Engine
   |
   |-- Attempt to open AsciiArt.txt
        |
        |-- Exists? --- yes --> load lines & compute widths
        |                   no
        |
        |-- report missing to main.cpp
        |
        |-- wait for main command to generate file
        |
        |-- create AsciiArt.txt
        |-- copy content from Default_Ascii_Art.txt
        |-- reload & validate
        |
        |-- report success to main.cpp
```

---

### 4. Design Constraints

* Must **not** render art to console.
* Must **not** access system information.
* Must **not** apply colors or formatting.
* Only manages **ASCII file existence, content, and loading**.
* Must produce **safe, terminal-renderable output**.
* Can be reused for future **themes or presets** without modifying main.cpp.

---

### 5. Summary

`AsciiArtControlEngine.cpp` is the **file manager and generator** for ASCII art. It ensures:

* ASCII art **always exists**.
* Default art is **bundled in source code** and used when needed.
* Main can safely render ASCII art without worrying about missing files or alignment.

---



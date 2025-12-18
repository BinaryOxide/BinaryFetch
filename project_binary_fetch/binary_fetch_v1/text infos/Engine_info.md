

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



# Linux System-Wide Autocomplete Daemon (C++)

A system-wide autocomplete engine for Linux that completes words based on a typed prefix when a hotkey is pressed.
It runs as a lightweight background daemon, has no GUI, and works across all X11 applications including terminals, editors, and browsers.

---

## What This Project Does

- Listens to global keyboard input on Linux
- Tracks the current word prefix in real time
- On hotkey press, autocompletes the word in the active window
- Works across all applications without focus stealing or application integration

---

## Demo

<img width="830" height="568" alt="image" src="https://github.com/user-attachments/assets/008c2eae-e24a-4dc8-a96e-8e7cbca18d84" />

video demo:https://drive.google.com/file/d/1P6DnqR_j8rX4Q2K2mRT_UgJTZj0w7neS/view?usp=sharing

---

## How It Works

1. Global keystroke capture using the X11 RECORD extension
2. Prefix matching using a compressed radix trie
3. Instant suggestion selection using precomputed top-K results
4. Text injection into the active window using X11 XTest

---

## Technical Highlights

- Radix trie (compressed trie) for fast prefix lookup
- Memory-mapped dictionary (mmap) for near-instant startup
- Top-K suggestions per node for constant-time completion
- Non-intrusive design with no input grabs or focus changes

---

## Performance Characteristics

- Prefix lookup complexity: O(L)
- Lower memory footprint compared to a standard trie
- Suitable for continuous background execution

---

## Build

make

---

## Run

sudo ./daemon

---

## Project Structure

├── daemon_xrecord.cpp     # Global key listener and text injection  
├── build_trie_blob.cpp    # Dictionary preprocessing tool  
├── MMapTrieLoader.cpp     # Memory-mapped trie loader  
├── MMapTrieLoader.h  
├── corpus.txt             # Example vocabulary  
├── Makefile  
└── README.md  

---

## Why No GUI?

This project intentionally avoids a GUI to focus on:

- System-level keyboard interception
- Low-latency autocomplete logic
- Application-agnostic behavior

A GUI layer can be added on top later if needed.

---

## Possible Extensions

- GUI suggestion popup
- Adaptive learning from user typing patterns
- Configurable hotkeys
- Wayland support

---

## License

MIT License

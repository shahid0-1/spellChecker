This project is a simple real-time spell checker text editor built using C++ and SFML (Simple and Fast Multimedia Library). It allows users to type text in an interactive UI, highlights misspelled words, and provides suggestions for correction.

Features
Dynamic text input with spell checking:
The editor detects incorrect words in real time and highlights them.

Suggestions for misspelled words:
A list of up to 5 suggestions is displayed based on prefix matching and Levenshtein distance.

Word wrapping and text formatting:
The input box supports automatic word wrapping for long lines.

Interactive UI with SFML:
Customizable UI components including an input box, suggestion box, and title text.

How It Works
Input Handling:
The user types into the input box, and the text is processed dynamically using SFML's event system (sf::Event::TextEntered).

Spell Checking:

Each word is split and checked against a pre-loaded dictionary.
Misspelled words are highlighted in real time.
Suggestions Generation:

Prefix-based suggestions are generated using a Trie data structure.
Suggestions are ranked using the Levenshtein distance to ensure relevance.
UI Rendering:
The SFML window continuously updates and renders the input text, highlighted words, and suggestions.

Core Components
SpellChecker Class:

Loads the dictionary from a file.
Checks if a word is correct.
Provides suggestions for incorrect words.
Trie Data Structure:
Efficiently stores the dictionary and retrieves suggestions based on prefixes.

Levenshtein Distance Algorithm:
Computes the minimum number of edits (insertions, deletions, or substitutions) required to transform one word into another, used for ranking suggestions.

Requirements
C++17 or later
SFML 2.5.1 or later
A dictionary file (dictionary.txt) containing valid words.
Font file (arial.ttf) for rendering text.
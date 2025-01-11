#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

// ==========================
// Helper Functions
// ==========================
string toLowerCase(const string& word)
{
    string lowerWord;
    for (char c : word)
    {
        lowerWord += tolower(static_cast<unsigned char>(c));
    }
    return lowerWord;
}

vector<string> splitWords(const string& text)
{
    vector<string> words;
    string currentWord;
    for (char c : text)
    {
        if (isspace(c))
        {
            if (!currentWord.empty())
            {
                words.push_back(currentWord);
                currentWord.clear();
            }
        }
        else
        {
            currentWord += c;
        }
    }
    if (!currentWord.empty())
    {
        words.push_back(currentWord);
    }
    return words;
}

string wrapText(const string& text, const sf::Font& font, unsigned int fontSize, float maxWidth)
{
    string wrappedText;
    string currentLine;
    sf::Text sfText("", font, fontSize);

    for (const char& c : text)
    {
        currentLine += c;
        sfText.setString(currentLine);
        if (sfText.getLocalBounds().width > maxWidth)
        {
            auto lastSpace = currentLine.find_last_of(' ');
            if (lastSpace != string::npos)
            {
                wrappedText += currentLine.substr(0, lastSpace) + "\n";
                currentLine = currentLine.substr(lastSpace + 1);
            }
            else
            {
                wrappedText += currentLine + "\n";
                currentLine.clear();
            }
        }
    }
    wrappedText += currentLine; // Add the remaining text
    return wrappedText;
}

// ==========================
// Levenshtein Distance
// ==========================
int levenshteinDistance(const string& a, const string& b)
{
    vector<vector<int>> dp(a.size() + 1, vector<int>(b.size() + 1));
    for (size_t i = 0; i <= a.size(); ++i)
        dp[i][0] = i;
    for (size_t j = 0; j <= b.size(); ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= a.size(); ++i)
    {
        for (size_t j = 1; j <= b.size(); ++j)
        {
            if (a[i - 1] == b[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                dp[i][j] = min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] }) + 1;
            }
        }
    }
    return dp[a.size()][b.size()];
}

// ==========================
// Trie Node Structure
// ==========================
struct TrieNode
{
    map<char, TrieNode*> children;
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {}
};

class Trie
{
private:
    TrieNode* root;

public:
    Trie()
    {
        root = new TrieNode();
    }

    void insert(const string& word)
    {
        TrieNode* currentNode = root;
        for (char c : word)
        {
            if (currentNode->children.find(c) == currentNode->children.end())
            {
                currentNode->children[c] = new TrieNode();
            }
            currentNode = currentNode->children[c];
        }
        currentNode->isEndOfWord = true;
    }

    void getWordsWithPrefix(TrieNode* node, string prefix, vector<string>& results)
    {
        if (node->isEndOfWord)
        {
            results.push_back(prefix);
        }
        for (const auto& child : node->children)
        {
            getWordsWithPrefix(child.second, prefix + child.first, results);
        }
    }

    vector<string> getSuggestions(const string& prefix)
    {
        TrieNode* currentNode = root;
        vector<string> results;

        for (char c : prefix)
        {
            if (currentNode->children.find(c) == currentNode->children.end())
            {
                return results; // No suggestions if prefix doesn't exist
            }
            currentNode = currentNode->children[c];
        }

        getWordsWithPrefix(currentNode, prefix, results);
        return results;
    }
};

// ==========================
// SpellChecker Class
// ==========================
class SpellChecker
{
private:
    unordered_set<string> dictionary;
    Trie trie;

public:
    SpellChecker(const string& dictionaryFile)
    {
        loadDictionary(dictionaryFile);
    }

    void loadDictionary(const string& dictionaryFile)
    {
        ifstream file(dictionaryFile);
        string word;
        if (file.is_open())
        {
            while (file >> word)
            {
                word = toLowerCase(word);
                dictionary.insert(word);
                trie.insert(word);
            }
            file.close();
        }
        else
        {
            cout << "Failed to open dictionary file: " << dictionaryFile << endl;
        }
    }

    bool isWordCorrect(const string& word)
    {
        return dictionary.find(toLowerCase(word)) != dictionary.end();
    }

    vector<string> getSuggestions(const string& word)
    {
        vector<string> suggestions;
        string lowerWord = toLowerCase(word);

        // Get prefix-based suggestions
        vector<string> prefixMatches = trie.getSuggestions(lowerWord);

        // Use Levenshtein distance to rank suggestions
        map<int, vector<string>> distanceMap;
        for (const auto& match : prefixMatches)
        {
            int distance = levenshteinDistance(lowerWord, match);
            distanceMap[distance].push_back(match);
        }

        // Collect suggestions sorted by distance
        for (const auto& entry : distanceMap)
        {
            for (const auto& suggestion : entry.second)
            {
                suggestions.push_back(suggestion);
                if (suggestions.size() >= 5)
                { // Limit to 5 suggestions
                    return suggestions;
                }
            }
        }
        return suggestions;
    }
};

// ==========================
// Main Function with SFML UI
// ==========================
int main()
{
    // Implementation remains the same as your original.
    // Ensure SFML is properly configured with "arial.ttf" and "dictionary.txt" present.
    SpellChecker spellChecker("dictionary.txt");

    sf::RenderWindow window(sf::VideoMode(800, 600), "Spell Checker Text Editor");
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cerr << "Error loading font!" << endl;
        return -1;
    }

    // Background and UI elements
    sf::RectangleShape background(sf::Vector2f(window.getSize()));
    background.setFillColor(sf::Color(50, 50, 50));

    sf::Text titleText("Spell Checker", font, 36);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setPosition(window.getSize().x / 2 - titleText.getLocalBounds().width / 2, 20);
    titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);

    // Input box and text
    sf::RectangleShape inputBox(sf::Vector2f(window.getSize().x - 20, 200)); // Increased height for multiple lines
    inputBox.setPosition(10, 80);
    inputBox.setFillColor(sf::Color(30, 30, 30));
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::White);

    sf::Text userInputText("", font, 24);
    userInputText.setFillColor(sf::Color::White);
    userInputText.setPosition(15, 80); // Adjusted padding

    // Add horizontal lines
    vector<sf::RectangleShape> inputBoxLines;
    float lineSpacing = 30.0f; // Line spacing based on font size and padding
    for (float y = inputBox.getPosition().y; y < inputBox.getPosition().y + inputBox.getSize().y; y += lineSpacing)
    {
        sf::RectangleShape line(sf::Vector2f(inputBox.getSize().x, 1)); // Horizontal line
        line.setPosition(inputBox.getPosition().x, y);
        line.setFillColor(sf::Color(100, 100, 100)); // Light gray color
        inputBoxLines.push_back(line);
    }

    sf::Text suggestionTitle("Suggestions:", font, 20);
    suggestionTitle.setFillColor(sf::Color::Yellow);
    suggestionTitle.setPosition(10, 140);

    sf::RectangleShape suggestionBox(sf::Vector2f(window.getSize().x - 20, 100));
    suggestionBox.setPosition(10, 160);
    suggestionBox.setFillColor(sf::Color(30, 30, 30));
    suggestionBox.setOutlineThickness(2);
    suggestionBox.setOutlineColor(sf::Color::Green);

    sf::Text suggestionText("", font, 20);
    suggestionText.setFillColor(sf::Color::Green);
    suggestionText.setPosition(10, 170);

    string userInput;
    vector<string> suggestions;

    // Function to update the input box size based on text entered
    auto updateInputBoxSize = [&]()
        {
            // Get the bounds of the text
            sf::FloatRect textBounds = userInputText.getLocalBounds();

            // Update the height of the input box based on text's height
            float newHeight = std::max(textBounds.height + 20.0f, 200.0f); // Ensure a minimum height

            // Update the input box size
            inputBox.setSize(sf::Vector2f(window.getSize().x - 20, newHeight));

            // Update the position of each horizontal line
            inputBoxLines.clear(); // Clear old lines
            for (float y = inputBox.getPosition().y; y < inputBox.getPosition().y + newHeight; y += lineSpacing)
            {
                sf::RectangleShape line(sf::Vector2f(inputBox.getSize().x, 1)); // Horizontal lin
                line.setPosition(inputBox.getPosition().x, y);
                line.setFillColor(sf::Color(100, 100, 100)); // Light gray color
                inputBoxLines.push_back(line);
            }
        };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode == 8 && !userInput.empty())
                { // Backspace
                    userInput.pop_back();
                }
                else if (event.text.unicode < 128 && event.text.unicode != 13)
                { // Ignore Enter
                    userInput += static_cast<char>(event.text.unicode);
                }
            }
        }

        vector<string> words = splitWords(userInput);
        string displayText;
        for (const auto& word : words)
        {
            if (!spellChecker.isWordCorrect(word))
            {
                displayText += "[" + word + "] ";
            }
            else
            {
                displayText += word + " ";
            }
        }

        userInputText.setString(wrapText(displayText, font, 24, inputBox.getSize().x - 10));

        updateInputBoxSize(); // Update the input box size after text changes

        if (!words.empty())
        {
            string lastWord = words.back();
            if (!spellChecker.isWordCorrect(lastWord))
            {
                suggestions = spellChecker.getSuggestions(lastWord);
            }
            else
            {
                suggestions.clear();
            }
        }

        string suggestionDisplay;
        for (const auto& suggestion : suggestions)
        {
            suggestionDisplay += suggestion + "\n";
        }
        suggestionText.setString(wrapText(suggestionDisplay, font, 20, suggestionBox.getSize().x - 10));

        float lineHeight = 25; // Adjust height based on font size
        float newHeight = suggestions.size() * lineHeight + 10;
        suggestionBox.setSize(sf::Vector2f(suggestionBox.getSize().x, newHeight));

        suggestionTitle.setPosition(10, inputBox.getPosition().y + inputBox.getSize().y + 10);
        suggestionBox.setPosition(10, suggestionTitle.getPosition().y + 30);
        suggestionText.setPosition(10, suggestionBox.getPosition().y + 10);

        window.clear();
        window.draw(background);
        window.draw(titleText);
        window.draw(inputBox);
        window.draw(userInputText);
        window.draw(suggestionTitle);
        window.draw(suggestionBox);
        window.draw(suggestionText);
        window.display();
    }
    return 0;
}

#include <iostream>
#include <string>
#include <stack>
#include <unordered_map> // Stores variable values for evaluation
#include <cctype> // Functions for character classification
#include <sstream> // Splits postfix into tokens for processing
#include <stdexcept> // Supports exception handling
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace std;

int precedence(char op) 
{
	if (op == '+' || op == '-')
	{
		return 1; // Lowest precedence, following order of operations
	}
	if (op == '*' || op == '/' || op == '%')
	{
		return 2; // Meddium
	}
	if (op == '^')
	{
		return 3; // Highest priority
	}
	return 0;
}
bool isexpressionValid(const string& expr)
{
	stack<char> parentheses;
	for (char c : expr)
	{
		// check for valid characters, isalnum(c) is true, letter or digit,so negate it
		// if (c) is not a letter or digit, AND not among operators, print false
		if (!isalnum(c) && c != '+' && c != '-' && c != '*' && c != '/' && c
			!= '%' && c != '^' && c != '(' && c != ')' && c != ' ') {
			return false;
		}
		if (c == '(')
		{
			parentheses.push(c);
		}
		if (c == ')')
		{
			if (parentheses.empty())
			{
				return false; // Closing parenthesis without an opening match
			}
			parentheses.pop();
		}
	}
	return parentheses.empty(); // Ensure all opening parentheses have matching closing ones
}

// Function to convert an infix expression to a postfix expression
string infixToPostfix(const string& infix) {
	if (!isexpressionValid(infix)) {
		return "Invalid Expression";
	}

	stack<char> operators; // Stack to store operators
	stringstream output; // To build the postfix expression

	for (size_t i = 0; i < infix.size(); i++) {
		char c = infix[i];

		// If the character is an operand (letter or number), add it to the output
		if (isalnum(c))
		{
			output << c;

			// This loop is for multi-character operands, ex: (AB14),processed as single unit, not seperate characters.
			while (i + 1 < infix.size() && isalnum(infix[i + 1])) // i + 1 < infix.size(), stay within string's bounds.
				// isalnum(infix[i + 1])): Checks if the next character is letter or digit.
			{
				i++; // Move to next character in operand
				output << infix[i]; // keeps the multi-character operand intact.
			}
			output << " ";
		}
		else if (c == '(')
		{
			operators.push(c); // Push opening parenthesis onto the stack
		}
		else if (c == ')')
		{
			// Pop from stack until an opening parenthesis is found
			while (!operators.empty() && operators.top() != '(')
			{
				output << operators.top() << " "; // so operators are added in the correct order to output
				operators.pop();
			}
			operators.pop(); // Remove '(' from the stack
		}
		else
		{
			// Pop from stack while precedence of top operator is greater or equal to current operator (c)
			while (!operators.empty() && precedence(operators.top()) >= precedence(c))
			{
				output << operators.top() << " ";
				operators.pop();
			}
			operators.push(c); // After popping higher precedence operators, Push the current operator onto the stack
		}
	}

	// Pop remaining operators in the stack
	while (!operators.empty())
	{
		output << operators.top() << " ";
		operators.pop();
	}

	return output.str(); // Return the final postfix expression, retrieves entire string stored in the stringstream object.
}

// Function to evalaute our postfix
int evaluatepostFix(const string& postfix, unordered_map<string, int>& values)
{
	if (postfix == " Invalid Expression")
	{
		return 0;
	}
	stack<int> operands; // Creating a stack (operands), it will store numbers as they get processed
	stringstream ss(postfix); // using stringstream(ss) to split postfix into individual tokens (operands & operators)
	string token; // To temporarily hold each extracted operand or operator

	// Looping through each token in postfix expression, extracts numbers, variables or operators.
	while (ss >> token) // ss >> token will extract all tokens one by one from the string
	{
		// If token is number or variable, look up its value in values and push it onto the stack
		if (isdigit(token[0]) || isalpha(token[0]))
		{
			operands.push(values[token]);
		}
		else
		{
			// If token is operator, pop the two values from stack, (b first, then a)
			int b = operands.top();
			operands.pop();
			int a = operands.top();
			operands.pop();

			if (token == "+")
			{
				operands.push(a + b);
			}
			else if (token == "-")
			{
				operands.push(a - b);
			}
			else if (token == "*")
			{
				operands.push(a * b);
			}
			else if (token == "/")
			{
				if (b == 0)
				{
					cout << " Division by 0 is not possible. " << endl;
					return 0;
				}
				operands.push(a / b);
			}
			else if (token == "%")
			{
				operands.push(a % b);
			}
			else if (token == "^")
			{
				int result = 1;
				for (int i = 0; i < b; i++) // loop to multiply a by itself (b) amount of times
				{
					result *= a;
				}
				operands.push(result);
			}
		}
	}
	return operands.top(); // The final result remains at the top of the stack.
}

// Function to run the graphical user interface (GUI)
void runGUI()
{
	// Step 1: Create a window with dimensions 600x400 pixels and title
	sf::RenderWindow window(sf::VideoMode(600, 400), "Infix to Postfix Calculator");

	// Step 2: Load a font for displaying text
	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) 
	{
		cout << "Error: Could not load font file 'arial.ttf'!" << endl;
		return; // Exit function if font fails to load
	}

	// Step 3: Create UI Labels for Instructions
	sf::Text inputLabel("Enter Infix Expression:", font, 20);
	inputLabel.setPosition(20, 20); // Set position on window

	sf::Text outputLabel("Postfix Expression:", font, 20);
	outputLabel.setPosition(20, 120);

	sf::Text resultLabel("Result:", font, 20);
	resultLabel.setPosition(20, 220);

	// Step 4: Create UI Texts for Displaying User Input & Results
	string userInput; // Stores user's infix expression
	sf::Text userInputDisplay("", font, 20);
	userInputDisplay.setPosition(20, 50);

	sf::Text postfixDisplay("", font, 20);
	postfixDisplay.setPosition(20, 150);

	sf::Text resultDisplay("", font, 20);
	resultDisplay.setPosition(20, 250);

	// Step 5: Create a "Clear" Button
	sf::Text clearButton("Clear", font, 20);
	clearButton.setPosition(500, 350);
	clearButton.setFillColor(sf::Color::Red); // Set text color to red

	// Main loop: Keeps window open and running
	while (window.isOpen()) 
	{
		sf::Event event;

		// Step 6: Process Events (User Interactions)
		while (window.pollEvent(event))
		{
			// If user clicks the "X" button, close the window
			if (event.type == sf::Event::Closed) 
			{
				window.close();
			}

			// Step 7: Handle User Typing Input
			if (event.type == sf::Event::TextEntered)
			{
				// If user presses backspace, remove last character
				if (event.text.unicode == 8 && !userInput.empty()) 
				{
					userInput.pop_back();
				}
				// Otherwise, add valid characters to input
				else if (event.text.unicode < 128 && event.text.unicode != 8) 
				{
					userInput += static_cast<char>(event.text.unicode);
				}
				// Update the text display
				userInputDisplay.setString(userInput);
			}

			// Step 8: Detect "Enter" Key for Processing Expression
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) 
			{
				// Convert infix expression to postfix
				string postfix_expr = infixToPostfix(userInput);
				postfixDisplay.setString("Postfix Expression: " + postfix_expr);

				// If the conversion was successful, evaluate the expression
				if (postfix_expr != "Invalid Expression") 
				{
					unordered_map<string, int> values;

					// Step 9: Ask User to Enter Values for Variables
					cout << "Enter variable values (e.g., A 2 B 3 C 4), then press Enter:\n";
					string var;
					int val;

					while (cin >> var >> val) 
					{
						values[var] = val; // Store variable and value in a map
					}

					cin.clear(); // Reset input buffer
					cin.ignore(); // Ignore leftover input

					// Step 10: Evaluate the postfix expression and display the result
					int result = evaluatepostFix(postfix_expr, values);
					resultDisplay.setString("Result: " + to_string(result));
				}
				else 
				{
					resultDisplay.setString("Invalid Expression!"); // Display error message
				}
			}

			// Step 11: Detect Backspace Key for Deleting Input
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Backspace) 
			{
				if (!userInput.empty()) 
				{
					userInput.pop_back(); // Remove last character
					userInputDisplay.setString(userInput); // Update display
				}
			}

			// Step 12: Handle "Clear" Button Click
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{ // Detect left mouse click
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);

					// If mouse click is inside "Clear" button area, reset everything
					if (clearButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) 
					{
						userInput.clear();
						userInputDisplay.setString("");
						postfixDisplay.setString("");
						resultDisplay.setString("");
					}
				}
			}
		}

		// Step 13: Clear and Redraw Everything on Window
		window.clear(sf::Color::White); // White background
		window.draw(inputLabel);
		window.draw(userInputDisplay);
		window.draw(outputLabel);
		window.draw(postfixDisplay);
		window.draw(resultLabel);
		window.draw(resultDisplay);
		window.draw(clearButton);
		window.display(); // Refresh the window
	}
}

int main()
{
	char choice;
	cout << " Would you like to run in GUI mode? (y/n): " << endl;
	cin >> choice;

	if (choice == 'y' || choice == 'Y')
	{
		runGUI();
	}
	else
	{
		string infix_expr;
		cout << " Enter The Infix Expression: " << endl;
		getline(cin, infix_expr);

		string postfix_expr = infixToPostfix(infix_expr);
		cout << " The Postfix Expression is : " << postfix_expr << endl;

		if (postfix_expr != "Invalid Expression")
		{
			unordered_map<string, int> values; // Cretes a hash map to store variable values, A,B,C can be repalced with num

			cout << " Please enter values for variables, (e.g., A 2 B 3 C 4): " << endl;

			string var; int val;


			while (cin >> var >> val) // cin >> var, reads variable name, cin >> val, reads numeric value assigned to variable
			{
				values[var] = val; // Stores variable(var) as key, and its value(val) in the unordered_map(values)
			}
			cin.clear(); // resets input state
			cin.ignore(); // removes leftover characters

			int result = evaluatepostFix(postfix_expr, values);
			cout << " Evaluation Result: " << result << endl;
		}
		else
		{
			cout << " You have entered an Invalid expression " << endl;
		}
		return 0;
	}
}


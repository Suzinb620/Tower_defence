export module LoginState;
import <SFML/Graphics.hpp>;
import <filesystem>;
import <iostream>;
import <fstream>;
import <regex>;
import <map>;
import ResourceManager;
import MainMenuState;
import TextBox;
import Button;
import State;

export class LoginState : public State
{
private:
	//Variables:
	std::string username;
	std::string password;
	std::map<std::string, Button*> buttons;
	std::map<std::string, TextBox*> textBoxes;
	sf::Text text;
	std::string dataPath;

public:
	//Constructors/destructors:
	LoginState(std::stack<State*>* states) : State(states)
	{
		dataPath = std::filesystem::current_path().string() + "\\data.csv";
		initTexts();
		initTextBoxes();
		initButtons();
	}

	~LoginState()
	{
		for (auto& button : this->buttons)
			delete button.second;
		for (auto& textBox : this->textBoxes)
			delete textBox.second;
	}

private:
	//Initialization:
	void initTexts()
	{
		this->text.setFillColor(sf::Color::Red);
		this->text.setCharacterSize(25);
		this->text.setFont(*ResourceManager::getFont());
	}

	void initTextBoxes()
	{
		this->textBoxes.insert({ "username", new TextBox(300, 300, 700, 100, "Username:", sf::Color(180, 180, 180), sf::Color(140, 140, 140), ResourceManager::getFont()) });
		this->textBoxes.insert({ "password", new TextBox(300, 450, 700, 100, "Password:", sf::Color(180, 180, 180), sf::Color(140, 140, 140), ResourceManager::getFont()) });
	}

	void initButtons()
	{
		this->buttons.insert({ "logIn", new Button(100, 600, 300, 100, "Log in", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont()) });
		this->buttons.insert({ "Register", new Button(500, 600, 300, 100, "Register", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont()) });
		this->buttons.insert({ "quitGame", new Button(900, 600, 300, 100, "Quit", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont()) });
	}
	
public:
	//Update:
	void update(const float& deltaTime, const sf::Vector2f& mousePosition, sf::RenderWindow* window, sf::Event& ev) override
	{
		updateButtons(mousePosition, ev);
		updateTextBoxes(mousePosition, window, ev);
		updateEvent(window, ev);
		updateUsernameAndPassword();
	}

	void updateUsernameAndPassword()
	{
		this->username = this->textBoxes["username"]->getString();
		this->password = this->textBoxes["password"]->getString();
	}

	void updateButtons(const sf::Vector2f& mousePosition, sf::Event& ev)
	{
		for (const auto& button : this->buttons)
			button.second->update(mousePosition, ev);
	}

	void updateTextBoxes(const sf::Vector2f& mousePosition, sf::RenderWindow* window, sf::Event& ev)
	{
		for (const auto& textBox : this->textBoxes)
			textBox.second->update(mousePosition, window, ev);
	}

	void updateEvent(sf::RenderWindow* window, sf::Event& ev)
	{
		while (window->pollEvent(ev))
		{
			switch (ev.type)
			{
			case sf::Event::Closed:
				window->close();
				break;
			case sf::Event::MouseButtonReleased:
				if (ev.mouseButton.button == sf::Mouse::Left)
				{
					eventQuitButton(window);
					eventLoginButton(window);
					eventRegisterButton(window);
				}
				break;
			}
		}
	}
	//Events:
	void eventQuitButton(sf::RenderWindow* window)
	{
		if (this->buttons["quitGame"]->isPressable())
			quit();
	}

	void eventLoginButton(sf::RenderWindow* window)
	{
		if (this->buttons["logIn"]->isPressable())
		{
			if (checkDatabase())
			{
				quit();
				this->states->push(new MainMenuState(this->states));
			}
			else
			{
				this->text.setString("Nieprawidlowa nazwa uzytkownika lub haslo");
				this->text.setPosition(650 - this->text.getGlobalBounds().width / 2, 250);
			}
		}
	}

	void eventRegisterButton(sf::RenderWindow* window)
	{
		if (this->buttons["Register"]->isPressable())
		{
			if (checkUsername() && checkPassword())
			{
				if (checkDatabase())
				{
					this->text.setString("Nazwa uzytkownika lub haslo jest juz zajete");
					this->text.setPosition(650 - this->text.getGlobalBounds().width / 2, 250);
				}
				else
				{
					this->text.setString("Zarejestrowany! - zaloguj sie");
					this->text.setPosition(650 - this->text.getGlobalBounds().width / 2, 250);
					addToDatabase();
				}
			}
		}
	}

	//Render:
	void render(sf::RenderTarget* renderTarget) override
	{
		for (const auto& button : this->buttons)
			button.second->render(renderTarget);
		for (const auto& textBox : this->textBoxes)
			textBox.second->render(renderTarget);

		renderTarget->draw(this->text);
	}

	//Methods:
	bool checkUsername()
	{
		std::regex regex("^[a-zA-Z0-9]{3,}$");
		if (regex_match(this->username, regex))
			return true;
		else
		{
			this->text.setString("Nieprawidlowa nazwa uztykownika - dozwolone male i wielkie litery oraz cyfry - min.3 znaki");
			this->text.setPosition(650 - this->text.getGlobalBounds().width / 2, 250);
			return false;
		}
	}

	bool checkPassword()
	{
		std::regex regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*_+=-?<>]).{5,}$");
		if (regex_match(this->password, regex))
			return true;
		else
		{
			this->text.setString("Nieprawidlowe haslo - haslo musi zawierac mala i wielka litere, cyfre oraz znak specialny - min. 5 znakow");
			this->text.setPosition(650 - this->text.getGlobalBounds().width / 2, 250);
			return false;
		}
	}
	
	bool checkDatabase()
	{
		std::ifstream file(dataPath);
		if (std::filesystem::exists(dataPath))
		{
			if (file)
			{
				std::string line;
				while (std::getline(file, line))
				{
					if (line == username + ";" + password) 
						return true;
				}
				return false;
			}
			else throw std::exception("Nie mozna otworzyc pliku z danymi logowania\n");
		}
		else return false;

		file.close();
	}

	void addToDatabase()
	{
		std::ofstream file(dataPath, std::ios_base::app);

		if (file)		
			file << username << ";" << password << "\n";		
		else throw std::exception("Nie mozna otworzyc pliku z danymi logowania\n");

		file.close();
	}
};
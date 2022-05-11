export module MainMenuState;
import <SFML/Graphics.hpp>;
import <map>;
import <stack>;
import Button;
import State;
import <iostream>;
import GameState;
import SaveManager;
import ResourceManager;
import <atomic>;
import <mutex>;

export class MainMenuState : public State
{
private:
	//Variables:
	std::map<std::string, Button*> buttons;

public:
	//Constructors/destructors:
	MainMenuState(std::stack<State*>* states) : State(states) {	initButtons(); }

	~MainMenuState()
	{
		for (auto& button : this->buttons)
			delete button.second;
	}

private:
	//Initialization:
	void initButtons()
	{
		this->buttons.insert({ "newGame", new Button(490, 430, 300, 100, "New game", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont()) });
		this->buttons.insert({ "quitGame", new Button(490, 560, 300, 100, "Quit", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont()) });
		this->buttons.insert({ "continueGame", new Button(490, 300, 300, 100, "Continue", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont()) });
	}

public:
	//Update:
	void update(const float& deltaTime, const sf::Vector2f& mousePosition, sf::RenderWindow* window, sf::Event& ev) override
	{
		updateButtons(mousePosition, ev);
		updateEvent(window, ev);
		updateButtonsStatus();
	}

	void updateButtons(const sf::Vector2f& mousePosition, sf::Event& ev)
	{
		for (const auto& button : this->buttons)
			button.second->update(mousePosition, ev);	
	}

	void updateButtonsStatus()
	{
		if (SaveManager::isSaveExist())
			this->buttons["continueGame"]->activate();
		else
			this->buttons["continueGame"]->deactivate();
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
					eventButton(window);
				break;
			}
		}
	}

	//Events:
	void eventButton(sf::RenderWindow* window)
	{
		if (this->buttons["newGame"]->isPressable())
			this->states->push(new GameState(this->states));
		if (this->buttons["quitGame"]->isPressable())
			quit();
		if (this->buttons["continueGame"]->isPressable())
			this->states->push(new GameState(this->states, SaveManager::load()));
	}

	//Render:
	void render(sf::RenderTarget* renderTarget) override
	{
		for (const auto& button : this->buttons)		
			button.second->render(renderTarget);
	}
};
export module Engine;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <iostream>;
import <thread>;
import <mutex>;
import <stack>;
import ResourceManager;
import MainMenuState;
import LoginState;
import GameState;
import State;

export class Engine 
{
private:
	//Variables:
	sf::RenderWindow* window;
	sf::Event ev;
	std::stack<State*> states;
	sf::Clock deltaTimeClock;
	float deltaTime;
	sf::Vector2f mousePosition;

public:
	//Constructors/destructors:
	Engine() : window(new sf::RenderWindow(sf::VideoMode(1280, 960), "Tower defence", sf::Style::Titlebar | sf::Style::Close))
	{
		window->setFramerateLimit(144);
		ResourceManager::load();
		this->states.push(new LoginState(&this->states));
	}

	~Engine()
	{
		ResourceManager::unload();
		delete this->window;

		while (!this->states.empty())
		{
			delete this->states.top();
			this->states.pop();
		}
	}

private:
	//Update:
	void update()
	{
		this->updateMousePosition();
		this->updateDeltaTime();

		if (!this->states.empty())
		{
			this->states.top()->update(this->deltaTime, this->mousePosition, this->window, this->ev);

			if (this->states.top()->getQuit())
			{
				delete this->states.top();
				this->states.pop();
			}
		}
		else
			this->window->close();
	}

	void updateMousePosition() { this->mousePosition = this->window->mapPixelToCoords(sf::Mouse::getPosition(*this->window)); }
	void updateDeltaTime() { this->deltaTime = this->deltaTimeClock.restart().asSeconds(); }

	//Render:
	void render()
	{
		this->window->clear(sf::Color::Black);

		if (!this->states.empty())
			this->states.top()->render(this->window);

		this->window->display();
	}

public:
	//Methods:
	void run()
	{
		while (this->window->isOpen())
		{
			this->update();
			this->render();
		}
	}
};
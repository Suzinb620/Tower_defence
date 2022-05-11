export module State;
import <SFML/Graphics.hpp>;
import <filesystem>;
import <stack>;
import ResourceManager;

export class State
{
protected:
	//Variables:
	bool quitState;
	std::stack<State*>* states;

public:
	//Constructors/destructors:
	State(std::stack<State*>* states) : quitState(false), states(states) {}

	//Update/render:
	virtual void update(const float& deltaTime, const sf::Vector2f& mousePosition, sf::RenderWindow* window, sf::Event& ev) = 0;
	virtual void render(sf::RenderTarget* renderTarget) = 0;

	//Methods:
	void quit() { this->quitState = true; }

	//Accessor:
	bool getQuit() { return this->quitState; }
};
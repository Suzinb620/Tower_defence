export module TextBox;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <iostream>;

export class TextBox
{
private:
	//Variables:
	sf::RectangleShape rect;
	sf::RectangleShape rectOfTextBox;
	sf::Text text;
	sf::Text textBox;
	sf::Color idleColor;
	sf::Color activeColor;
	bool selected;

public:
	//Constructors/destructors:
	TextBox(float x, float y, float width, float height, std::string text, sf::Color idleColor, sf::Color activeColor, sf::Font* font) : selected(false), idleColor(idleColor), activeColor(activeColor)
	{
		initText(text, font, x, y, width, height);
		initRect(idleColor, x, y, width, height);
		initTextBox(font, x, y, width, height);
		initRectOfTextBox(idleColor, x, y, width, height);
	};

private:
	//Initialization:
	void initText(const std::string& text, sf::Font* font, const float& x, const float& y, const float& width, const float& height)
	{
		this->text.setString(text);
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(30);
		this->text.setFont(*font);
		this->text.setPosition(x + 10, y + (height / 2) - (this->text.getGlobalBounds().height / 2));
	}

	void initRect(const sf::Color& idleColor, const float& x, const float& y, const float& width, const float& height)
	{
		this->rect.setFillColor(idleColor);
		this->rect.setPosition(sf::Vector2f(x, y));
		this->rect.setSize(sf::Vector2f(this->text.getGlobalBounds().width + 20, height));
	}

	void initTextBox(sf::Font* font, const float& x, const float& y, const float& width, const float& height)
	{
		this->textBox.setFillColor(sf::Color::Black);
		this->textBox.setCharacterSize(30);
		this->textBox.setFont(*font);
		this->textBox.setPosition(x + this->text.getGlobalBounds().width + 25, y + (height / 2) - (this->text.getGlobalBounds().height / 2));
	}

	void initRectOfTextBox(const sf::Color& idleColor, const float& x, const float& y, const float& width, const float& height)
	{
		this->rectOfTextBox.setFillColor(idleColor);
		this->rectOfTextBox.setPosition(sf::Vector2f(x + this->text.getGlobalBounds().width + 20, y));
		this->rectOfTextBox.setSize(sf::Vector2f(width - this->text.getGlobalBounds().width - 20, height));
	}

public:
	//Update:
	void update(const sf::Vector2f& mousePosition, sf::RenderWindow* window, sf::Event& ev)
	{
		updateIsSelected(mousePosition);

		if (this->selected)
			updateEvent(window, ev);
	}

private:
	void updateIsSelected(const sf::Vector2f& mousePosition)
	{
		if (this->rectOfTextBox.getGlobalBounds().contains(mousePosition) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			setSelected();
			setColor(activeColor);
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			setUnselected();
			setColor(idleColor);
		}
	}

	void updateEvent(sf::RenderWindow* window, sf::Event& ev)
	{
		while (window->pollEvent(ev))
		{
			std::string textToDisplay = textBox.getString();

			if (ev.type == sf::Event::TextEntered)
				if (ev.text.unicode != 8 && ev.text.unicode != 9)
					textToDisplay += (char)ev.text.unicode;
				else if (ev.text.unicode == 8)
					textToDisplay = textToDisplay.substr(0, textToDisplay.length() - 1);

			textBox.setString(textToDisplay);
		}
	}

public:
	//Render:
	void render(sf::RenderTarget* renderTarget) 
	{
		renderTarget->draw(this->rect);
		renderTarget->draw(this->rectOfTextBox);
		renderTarget->draw(this->text);
		renderTarget->draw(this->textBox);
	}

	//Methods:
	void setColor(sf::Color color) { this->rectOfTextBox.setFillColor(color); }
	void setSelected() { this->selected = true; }
	void setUnselected() { this->selected = false; }

	//Accessors:
	bool isSelected() { return this->selected; }
	std::string getString() { return textBox.getString(); }
};
import Engine;
import <iostream>;

int main()
{
    try 
    {
        Engine game;
        game.run();
    }
    catch (const std::exception& exception) { std::cout << exception.what(); }

    return EXIT_SUCCESS;
}
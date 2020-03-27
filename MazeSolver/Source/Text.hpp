#pragma once

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class Text
{
public:
	inline Text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, 
		int posX, int posY, const SDL_Color& color = { 255, 255, 255 });

	inline void render();

	inline void clean();

	inline void setColor(const SDL_Color& color);

	inline void setText(const std::string& color);

	inline SDL_Rect& getRect() { return rect; }
private:
	std::string text;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	TTF_Font* font;
	SDL_Color color;
	SDL_Rect rect;
};

inline Text::Text(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int posX, int posY, const SDL_Color& color) :
	text(text), renderer(renderer), texture(NULL), font(font), color(color), rect({posX, posY, 0, 0})
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
}

inline void Text::setColor(const SDL_Color& color)
{
	SDL_DestroyTexture(texture);

	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	this->color = color;
}

inline void Text::setText(const std::string& text)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	this->text = text;
}

inline void Text::render()
{
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

inline void Text::clean()
{
	SDL_DestroyTexture(texture);
}
#pragma once

#include<iostream>
#include<string>
#include<map>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>
#include "json.hpp"

enum sound_type
{
	SOUND_MUSIC = 0,
	SOUND_SFX = 1
};

using namespace std;

class AssetsManager
{
public:
	//Constructors
	static AssetsManager* Instance()
	{
		if (s_pInstance == 0)
		{
			s_pInstance = new AssetsManager();
		}

		return s_pInstance;
	}

	AssetsManager() {
		Mix_OpenAudio(22050, AUDIO_S16, 2, (4096 / 2));
	};
	~AssetsManager() {
		Mix_CloseAudio();
	};

	void loadAssets();
	void loadAssetsJson();

	//TEXTURES
	//Load the image with the fileName, and assigns an id.
	bool loadTexture(string fileName, string id, SDL_Renderer* pRenderer);

	void draw(string id, int x, int y, int width, int height,
		SDL_Renderer* pRenderer, SDL_RendererFlip = SDL_FLIP_NONE); //draw the static image.

	void drawFrame(string id, int x, int y, int width, int height,
		int currentRow, int currentFrame, SDL_Renderer* pRenderer, double angle, int alpha, SDL_RendererFlip flip = SDL_FLIP_NONE); //draw image sprite

	void drawFrameScl(string id, int x, int y, int srcWidth, int srcHeight, int destWidth, int destHeight,
		int currentRow, int currentFrame, SDL_Renderer* pRenderer, double angle, int alpha, SDL_RendererFlip flip = SDL_FLIP_NONE); //draw image sprite

	void drawTile(std::string id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer);

	void drawTileScl(std::string id, int margin, int spacing, int x, int y, int srcWidth, int srcHeight, int destWidth, int destHeight, int currentRow, int currentFrame, SDL_Renderer *pRenderer);

	void clearFromTextureMap(string id); //delete SDL_Texture from the map
	void clearAllTextures();

	//SOUND / MUSIC

	bool loadSound(std::string fileName, std::string id, sound_type type);
	void playSound(std::string id, int loop);
	void playMusic(std::string id, int loop);
	void clearSoundsMusic();

	//FONTS
	bool loadFont(const string &fileName, string id, int size);

	SDL_Texture* renderText(const std::string &message, const std::string &font,
		SDL_Color color, SDL_Renderer *renderer);

	//usage: AssetsManager::Instance()->Text("hola mundo 1", "font", 50, 50, SDL_Color({ 190,34,12,0 }), Game::Instance()->getRenderer());
	void Text(const std::string &message, const std::string &font, int x, int y, SDL_Color color, SDL_Renderer *renderer);

	TTF_Font* getFont(std::string f) {
		return m_fonts[f];
	}

	SDL_Texture* getTexture(std::string t) {
		return m_textureMap[t];
	}

	/*void clearFonts() {
		auto iter = m_fonts.begin();
		while (iter != m_fonts.end()) {
			TTF_CloseFont(iter->second);
			iter++;
		}
	}*/

	void clearFonts() {
		for (auto& f : m_fonts) {
			TTF_CloseFont(f.second);
		}
		m_fonts.clear();
	}

	SDL_Renderer* renderer;

private:
	map<string, SDL_Texture*> m_textureMap; //map with all the SDL_Textures

	map<string, Mix_Chunk*> m_sfxs;
	map<string, Mix_Music*> m_music;

	map<string, TTF_Font*> m_fonts;

	static AssetsManager* s_pInstance;
};


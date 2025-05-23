#pragma once
#include <SFML/Graphics.hpp>
#include <stack>

namespace FRE {

	class Canvas
	{
	public:
		Canvas();
		Canvas(unsigned int width, unsigned height, bool transparant);
		void Update(const sf::RenderWindow& window, const sf::View& view);
		void Display();
		void DrawLine(const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color);
		void Clear(const sf::Color& color);
		sf::Vector2u GetSize();
		void SetSize(unsigned int width, unsigned int height);
		sf::Sprite& GetSprite();
		sf::RenderTexture& GetRenderTexture();

		bool ExportToPNG(const std::string& filename);
		bool ExportToJPG(const std::string& filename);
		bool ExportToBMP(const std::string& filename);

		void CreateTransparencyPattern();
		void DrawTransparencyPattern(sf::RenderTarget& target);
		void SetTransparent(bool transparent);
		bool IsTransparent() const;

		// Undo System
		void SaveState();
		bool Undo();
		bool CanUndo() const;

		void BeginDrawOperation();
		void EndDrawOperation();

		void Rotate90();
		void RotateCounterClockwise90();
		void Rotate180();
		void FlipHorizontal();
		void FlipVertical();

	private:
		sf::RenderTexture m_RenderTexture;
		sf::Vector2u m_Size;
		std::optional<sf::Sprite> m_Sprite;

		sf::Texture m_TransparencyPatternTexture;
		std::optional<sf::Sprite> m_TransparencyPatternSprite;
		bool m_HasTransparencyPattern = false;
		bool m_Transparent = false;

		struct CanvasState {
			sf::Image image;
			sf::Vector2u size;
		};

		std::stack<CanvasState> m_UndoStack;
		bool m_IsDrawingOperation = false;
	};

}


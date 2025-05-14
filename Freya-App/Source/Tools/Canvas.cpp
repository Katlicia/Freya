#include "Canvas.h"

using namespace FRE;

Canvas::Canvas()
	: m_Size(800, 600)
{
	if (!m_RenderTexture.resize({ m_Size.x, m_Size.y }))
	{
		throw std::runtime_error("Failed to create RenderTexture");
	}
	m_RenderTexture.clear(sf::Color::White);
	m_Sprite.emplace(m_RenderTexture.getTexture());
}

Canvas::Canvas(unsigned int width, unsigned height) : m_Size({ width, height })
{
	if (!m_RenderTexture.resize({ m_Size.x, m_Size.y }))
	{
		throw std::runtime_error("Failed to create RenderTexture");
	}
	m_RenderTexture.clear(sf::Color::White);
	m_Sprite.emplace(m_RenderTexture.getTexture());
}

void Canvas::DrawLine(const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color = sf::Color::Black)
{
	sf::Vertex line[] = {
		sf::Vertex(start, color),
		sf::Vertex(end, color)
	};
	m_RenderTexture.draw(line, 2, sf::PrimitiveType::Lines);
}

void Canvas::Clear(const sf::Color& color)
{
	m_RenderTexture.clear(color);
}

void Canvas::Display()
{
	m_RenderTexture.display();
}

sf::Vector2u Canvas::GetSize()
{
	return m_Size;
}

sf::Sprite& Canvas::getSprite()
{
	if (!m_Sprite.has_value())
	{
		throw std::runtime_error("Sprite is not initialized!");
	}
	return *m_Sprite;
}

sf::RenderTexture& Canvas::getRenderTexture()
{
	return m_RenderTexture;
}

void Canvas::Update(const sf::RenderWindow& window, const sf::View& view)
{
	Display();
	// Create sprite if it doesn't exist, update if does exist
	if (!m_Sprite.has_value())
		m_Sprite.emplace(m_RenderTexture.getTexture());
	else
		m_Sprite->setTexture(m_RenderTexture.getTexture(), true);

	// Allign canvas to the center of the view
	sf::Vector2f viewCenter = window.getView().getCenter();
	sf::Vector2f canvasSizeF = static_cast<sf::Vector2f>(m_RenderTexture.getSize());
	sf::Vector2f offset;
	offset.x = viewCenter.x - (canvasSizeF.x / 2.f);
	offset.y = viewCenter.y - (canvasSizeF.y / 2.f);
	m_Sprite->setPosition(offset);
}

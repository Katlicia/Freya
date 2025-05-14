////#include "SFML/Graphics.hpp"
////#include "imgui.h"
////#include "imgui-SFML.h"
////#include <vector>
////
////int main()
////{
////    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML works!");
////    window.setFramerateLimit(120);
////    ImGui::SFML::Init(window);
////
////    // Hold points
////    std::vector<sf::CircleShape> points;
////
////    bool circleExists = true;
////    float circleRadius = 200.f;
////    int circleSegments = 100;
////	float circleColor[3] = { (float)204 / 255, (float)77 / 255, (float)5 / 255 };
////
////    sf::CircleShape shape(circleRadius, circleSegments);
////    shape.setFillColor(sf::Color((int)
////        (circleColor[0] * 255), 
////        (int)(circleColor[1] * 255), 
////        (int)(circleColor[2] * 255)));
////    shape.setPosition({ 400, 400 });
////
////    sf::Clock deltaClock;
////
////    while (window.isOpen())
////    {
////        while (const std::optional event = window.pollEvent())
////        {
////			ImGui::SFML::ProcessEvent(window, *event);
////            if (event->is<sf::Event::Closed>())
////                window.close();
////
////            //if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
////			if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
////			{
////				if (mouseEvent->button == sf::Mouse::Button::Left)
////				{
////					// Get mouse position
////                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
////					// Create a circle shape at the mouse position
////                    sf::CircleShape dot(2); // 2 piksel çapýnda küçük bir daire
////                    dot.setPosition({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) });
////                    dot.setFillColor(sf::Color::White);
////                    points.push_back(dot);
////				}
////			}
////
////        }
////		ImGui::SFML::Update(window, deltaClock.restart());
////
////        ImGui::Begin("Window Title");
////		ImGui::Text("Window Text!");
////        ImGui::Checkbox("Circle", &circleExists);
////        ImGui::SliderFloat("Radius", &circleRadius, 100.f, 300.f);
////		ImGui::SliderInt("Segments", &circleSegments, 3, 200);
////		ImGui::ColorEdit3("Color Circle", circleColor);
////        ImGui::End();
////
////        shape.setRadius(circleRadius);
////        shape.setPointCount(circleSegments);
////		shape.setFillColor(sf::Color((int)
////			(circleColor[0] * 255),
////			(int)(circleColor[1] * 255),
////			(int)(circleColor[2] * 255)));
////        shape.setOrigin({ circleRadius, circleRadius });
////
////        window.clear(sf::Color(18, 33, 43));
////        if (circleExists) {
////            //window.draw(shape);
////        }
////
////		for (auto& point : points)
////		{
////			window.draw(point);
////		}
////
////		ImGui::SFML::Render(window);
////        window.display();
////    }
////	ImGui::SFML::Shutdown(window);
////    return 0;
////}
//
////#include <SFML/Graphics.hpp>
////#include <vector>
////#include <cmath>
////
////int main()
////{
////    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML - Serbest Cizim");
////    window.setFramerateLimit(120); // Daha akýcý çizim için FPS limiti
////
////    std::vector<std::vector<sf::Vertex>> tumCizimler;
////	std::vector<sf::Vertex> aktifCizim; // cizim için vertex dizisi
////    bool cizimModu = false;
////
////    while (window.isOpen())
////    {
////        while (const std::optional event = window.pollEvent())
////        {
////         if (event->is<sf::Event::Closed>())
////             window.close();
////            // Basýlýnca çizim baþlasýn
////         if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
////         {
////             if (mouseEvent->button == sf::Mouse::Button::Left)
////             {
////                 cizimModu = true;
////                 aktifCizim.clear();
////				 sf::Vector2f poz = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////				 aktifCizim.push_back(sf::Vertex(poz, sf::Color::Black));
////             } 
////         }
////         if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>())
////
////            cizimModu = false;
////		 if (!aktifCizim.empty()) tumCizimler.push_back(aktifCizim);
////
////    }
////
////        if (cizimModu)
////        {
////            sf::Vector2f yeniPoz = window.mapPixelToCoords(sf::Mouse::getPosition(window));
////
////            // Eðer pozisyon deðiþtiyse yeni çizgi noktasý ekle
////            if (aktifCizim.empty() || aktifCizim.back().position != yeniPoz)
////            {
////				aktifCizim.push_back(sf::Vertex(yeniPoz, sf::Color::Black));
////            }
////        }
////
////        window.clear(sf::Color::White);
////
////        for (const auto& cizim : tumCizimler) {
////            if (cizim.size() >= 1) {
////				window.draw(&cizim[0], cizim.size(), sf::PrimitiveType::LineStrip);
////            }
////        }
////
////		if (aktifCizim.size() >= 1) {
////			window.draw(&aktifCizim[0], aktifCizim.size(), sf::PrimitiveType::LineStrip);
////		}
////        window.display();
////    }
////
////    return 0;
////}
////
//// 
//
//#include <SFML/Graphics.hpp>
//#include <iostream>
//
//int main()
//{
//    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
//	/*std::cout << desktop.size.x << " " << desktop.size.y << std::endl;*/
//
//    sf::RenderWindow window(sf::VideoMode({ desktop.size.x, desktop.size.y }), "Freya", sf::Style::Default, sf::State::Windowed);
//    window.setFramerateLimit(120);
//
//    sf::View view1(sf::FloatRect({ 0.f, 0.f }, { float(desktop.size.x), float(desktop.size.y) })); // canvas boyutuyla ayný
//    window.setView(view1);
//
//    sf::RenderTexture canvas({ 800, 600}); // Tüm çizimleri tutacak yüzey
//    canvas.clear(sf::Color::Green);
//
//
//    sf::Texture canvasTexture = canvas.getTexture();
//    sf::Sprite canvasSprite(canvasTexture);
//    
//    sf::Vector2f viewCenter = window.getView().getCenter();
//    sf::Vector2f viewSize = window.getView().getSize();
//    sf::Vector2f canvasSizeF = static_cast<sf::Vector2f>(canvas.getSize());
//
//    sf::Vector2f offset;
//    offset.x = viewCenter.x - (canvasSizeF.x / 2.f);
//    offset.y = viewCenter.y - (canvasSizeF.y / 2.f);
//
//    canvasSprite.setPosition(offset);
//
//
//    bool cizimModu = false;
//    sf::Vector2f oncekiPoz;
//
//    while (window.isOpen())
//    {
//        while (const std::optional event = window.pollEvent())
//        {
//            if (event->is<sf::Event::Closed>())
//                window.close();
//
//            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
//            {
//                cizimModu = true;
//                oncekiPoz = window.mapPixelToCoords(sf::Mouse::getPosition(window), view1) - canvasSprite.getPosition();
//            }
//
//            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>())
//            {
//                cizimModu = false;
//            }
//            if (const auto* resized = event->getIf<sf::Event::Resized>())
//            {
//                // update the view to the new size of the window
//                //sf::FloatRect visibleArea({ 0.f, 0.f }, sf::Vector2f(resized->size));
//                //window.setView(sf::View(visibleArea));
//
//                sf::Vector2u newSize = resized->size;
//
//                // Center sabit kalsýn
//                sf::Vector2f currentCenter = view1.getCenter();
//                view1.setSize({ float(newSize.x), float(newSize.y) });
//                view1.setCenter(currentCenter);
//
//                window.setView(view1);
//            }
//        }
//
//        if (cizimModu)
//        {
//            sf::Vector2f yeniPoz = window.mapPixelToCoords(sf::Mouse::getPosition(window), view1) - canvasSprite.getPosition();
//
//            if (oncekiPoz != yeniPoz)
//            {
//                // Ýki nokta arasýnda çizgi oluþtur
//                sf::Vertex line[] =
//                {
//                    sf::Vertex(oncekiPoz, sf::Color::Black),
//                    sf::Vertex(yeniPoz, sf::Color::Black)
//                };
//                canvas.draw(line, 2, sf::PrimitiveType::Lines); // Canvas üzerine çiz
//
//                oncekiPoz = yeniPoz;
//            }
//        }
//
//        // Güncel canvas'ý bir sprite olarak ekrana çiz
//        canvas.display();
//        sf::Texture canvasTexture = canvas.getTexture();
//        canvasSprite.setTexture(canvasTexture);
//
//        sf::Vector2f viewCenter = view1.getCenter();
//        sf::Vector2f canvasSizeF = static_cast<sf::Vector2f>(canvas.getSize());
//
//        sf::Vector2f offset;
//        offset.x = viewCenter.x - (canvasSizeF.x / 2.f);
//        offset.y = viewCenter.y - (canvasSizeF.y / 2.f);
//
//
//        canvasSprite.setPosition(offset);
//
//        window.clear(sf::Color::White);
//        window.draw(canvasSprite); // Sadece 1 draw iþlemi
//        window.display();
//    }
//
//    return 0;
//}

//#include <SFML/Graphics.hpp>
//#include <iostream>
//
//int main()
//{
//    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
//
//    sf::RenderWindow window(sf::VideoMode({ desktop.size.x, desktop.size.y }), "Freya", sf::Style::Default);
//    window.setFramerateLimit(120);
//
//    sf::View view1(sf::FloatRect({ 0.f, 0.f }, { float(desktop.size.x), float(desktop.size.y) }));
//    window.setView(view1);
//
//    sf::RenderTexture canvas({ 800, 600 });
//    canvas.clear(sf::Color::Green);
//
//    sf::Sprite canvasSprite(canvas.getTexture());
//
//    bool cizimModu = false;
//    sf::Vector2f oncekiPoz;
//
//    while (window.isOpen())
//    {
//        while (const std::optional event = window.pollEvent())
//        {
//            if (event->is<sf::Event::Closed>())
//                window.close();
//
//            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
//            {
//                cizimModu = true;
//                oncekiPoz = window.mapPixelToCoords(sf::Mouse::getPosition(window), view1) - canvasSprite.getPosition();
//            }
//
//            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>())
//            {
//                cizimModu = false;
//            }
//
//            if (const auto* resized = event->getIf<sf::Event::Resized>())
//            {
//                sf::Vector2u newSize = resized->size;
//                sf::Vector2f currentCenter = view1.getCenter();
//                view1.setSize({ float(newSize.x), float(newSize.y) });
//                view1.setCenter(currentCenter);
//                window.setView(view1);
//            }
//        }
//
//        if (cizimModu)
//        {
//            sf::Vector2f yeniPoz = window.mapPixelToCoords(sf::Mouse::getPosition(window), view1) - canvasSprite.getPosition();
//                
//            if (oncekiPoz != yeniPoz)
//            {
//                sf::Vertex line[] =
//                {
//                    sf::Vertex(oncekiPoz, sf::Color::Black),
//                    sf::Vertex(yeniPoz, sf::Color::Black)
//                };
//                canvas.draw(line, 2, sf::PrimitiveType::Lines);
//                oncekiPoz = yeniPoz;
//            }
//        }
//
//        // Canvas'ý güncelle ve sprite'a referansla baðla
//        canvas.display();
//        canvasSprite.setTexture(canvas.getTexture(), true);  // true: yeniden boyutlandýr
//
//        // Yeni ekran boyutuna göre canvas'ý ortala
//        sf::Vector2f viewCenter = view1.getCenter();
//        sf::Vector2f canvasSizeF = static_cast<sf::Vector2f>(canvas.getSize());
//        sf::Vector2f offset;
//        offset.x = viewCenter.x - (canvasSizeF.x / 2.f);
//        offset.y = viewCenter.y - (canvasSizeF.y / 2.f);
//        canvasSprite.setPosition(offset);
//
//        window.clear(sf::Color::White);
//        window.draw(canvasSprite);
//        window.display();
//    }
//
//    return 0;
//}

#include "Core/Application.h"
#include <iostream>
#include <exception>

int main()
{
    try
    {
        FRE::Application app;
        app.Run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return EXIT_FAILURE;
    }
}
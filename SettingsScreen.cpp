#include "SettingsScreen.h"
#include "Game.h"
#include <iostream>
#include <cmath>

// Constructor
SettingsScreen::SettingsScreen(Game *gameInstance) : Screen(gameInstance), selectedElement(2), mousePressed(false), draggingSlider(false), draggingSoundSlider(false), selectedResolution(0), volumePercent(1.0f), soundVolumePercent(1.0f)
{
  std::cout << "[SettingsScreen] Created" << std::endl;
  resolutions.push_back({800, 600, "800x600"});
  resolutions.push_back({1024, 768, "1024x768"});
  resolutions.push_back({1280, 720, "1280x720 (HD)"});
  resolutions.push_back({1366, 768, "1366x768"});
  resolutions.push_back({1920, 1080, "1920x1080 (Full HD)"});
  sf::Vector2u currentSize = game->getWindow()->getSize();
  for (size_t i = 0; i < resolutions.size(); i++)
  {
    if (resolutions[i].width == currentSize.x && resolutions[i].height == currentSize.y)
    {
      selectedResolution = static_cast<int>(i);
      break;
    }
  }
}

// Destructor
SettingsScreen::~SettingsScreen()
{
  std::cout << "[SettingsScreen] Destroyed" << std::endl;
}

// Navigate to this screen
void SettingsScreen::onEnter()
{
  std::cout << "[SettingsScreen] Entered" << std::endl;
  selectedElement = 2;
  mousePressed = false;
  draggingSlider = false;
  draggingSoundSlider = false;
  dropdownOpen = false;

  // Load current volumes from game
  volumePercent = game->getVolume() / 100.0f;
  soundVolumePercent = game->getSoundVolume() / 100.0f;
}

// Leave this screen
void SettingsScreen::onExit()
{
  std::cout << "[SettingsScreen] Exited" << std::endl;
  dropdownOpen = false;
}

// Event handling
void SettingsScreen::handleEvents()
{
  sf::RenderWindow *window = game->getWindow();

  while (const std::optional event = window->pollEvent())
  {
    if (event->is<sf::Event::Closed>())
    {
      std::cout << "[Settings] Close event" << std::endl;
      game->quit();
    }

    if (const auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
    {
      if (mousePress->button == sf::Mouse::Button::Left)
      {
        mousePressed = true;
      }
    }

    if (const auto *mouseRelease = event->getIf<sf::Event::MouseButtonReleased>())
    {
      if (mouseRelease->button == sf::Mouse::Button::Left)
      {
        mousePressed = false;
        draggingSlider = false;
        draggingSoundSlider = false;
      }
    }
  }
}

void SettingsScreen::update(float)
{
  sf::RenderWindow *window = game->getWindow();
  sf::Vector2u windowSize = window->getSize();
  sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
  sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);

  // Responsive panel bounds
  float panelWidth = windowSize.x * 0.68f;
  float panelHeight = windowSize.y * 0.65f;
  float panelX = windowSize.x / 2.0f - panelWidth / 2.0f;
  float panelY = windowSize.y * 0.20f;

  float sliderX = panelX + panelWidth * 0.36f;
  float sliderY = panelY + panelHeight * 0.26f;
  float sliderWidth = panelWidth * 0.43f;
  float sliderHeight = windowSize.y * 0.013f;
  sf::FloatRect sliderBounds({sliderX, sliderY}, {sliderWidth, sliderHeight});

  // Volume slider handle
  float handleX = sliderX + volumePercent * sliderWidth;
  float handleRadius = windowSize.y * 0.016f;
  sf::FloatRect handleBounds({handleX - handleRadius, sliderY - handleRadius}, {handleRadius * 2, handleRadius * 2});

  // Sound effects slider bounds (NEW - below music slider)
  float soundSliderY = panelY + panelHeight * 0.41f; // Below music slider
  sf::FloatRect soundSliderBounds({sliderX, soundSliderY}, {sliderWidth, sliderHeight});

  // Sound slider handle
  float soundHandleX = sliderX + soundVolumePercent * sliderWidth;
  sf::FloatRect soundHandleBounds({soundHandleX - handleRadius, soundSliderY - handleRadius}, {handleRadius * 2, handleRadius * 2});

  float dropdownX = panelX + panelWidth * 0.36f;
  float dropdownY = panelY + panelHeight * 0.56f; // Moved down from 0.41f
  float dropdownWidth = panelWidth * 0.43f;
  float dropdownHeight = windowSize.y * 0.065f;
  sf::FloatRect dropdownBounds({dropdownX, dropdownY}, {dropdownWidth, dropdownHeight});

  // Back button bounds
  float backButtonWidth = windowSize.x * 0.20f;
  float backButtonHeight = windowSize.y * 0.078f;
  float backButtonX = windowSize.x / 2.0f - backButtonWidth / 2.0f;
  float backButtonY = panelY + panelHeight + windowSize.y * 0.04f;
  sf::FloatRect backBounds({backButtonX, backButtonY}, {backButtonWidth, backButtonHeight});

  selectedElement = -1;

  if (handleBounds.contains(mousePos) || draggingSlider)
  {
    selectedElement = 0;
    if (mousePressed)
    {
      draggingSlider = true;
      float newVolume = (mousePos.x - sliderX) / sliderWidth;
      if (newVolume < 0.0f)
        newVolume = 0.0f;
      if (newVolume > 1.0f)
        newVolume = 1.0f;

      if (std::abs(volumePercent - newVolume) > 0.01f)
      {
        volumePercent = newVolume;
        applyVolume(volumePercent);
      }
    }
  }
  // Sound effects slider interaction
  else if (soundHandleBounds.contains(mousePos) || draggingSoundSlider)
  {
    selectedElement = 1;
    if (mousePressed)
    {
      draggingSoundSlider = true;
      // Update sound volume based on mouse X position
      float newSoundVolume = (mousePos.x - sliderX) / sliderWidth;
      if (newSoundVolume < 0.0f)
        newSoundVolume = 0.0f;
      if (newSoundVolume > 1.0f)
        newSoundVolume = 1.0f;

      if (std::abs(soundVolumePercent - newSoundVolume) > 0.01f)
      {
        soundVolumePercent = newSoundVolume;
        applySoundVolume(soundVolumePercent);
      }
    }
  }
  // Resolution dropdown interaction
  else if (dropdownBounds.contains(mousePos))
  {
    selectedElement = 2;
    if (mousePressed)
    {
      dropdownOpen = !dropdownOpen;
      mousePressed = false;
      std::cout << "[Settings] Dropdown toggled: " << (dropdownOpen ? "Open" : "Closed") << std::endl;
    }
  }
  else if (dropdownOpen)
  {
    for (size_t i = 0; i < resolutions.size(); i++)
    {
      float optionHeight = windowSize.y * 0.052f;
      float optionY = dropdownY + dropdownHeight + (i * optionHeight);
      sf::FloatRect optionBounds({dropdownX, optionY}, {dropdownWidth, optionHeight});

      if (optionBounds.contains(mousePos))
      {
        if (mousePressed && i != static_cast<size_t>(selectedResolution))
        {
          std::cout << "[Settings] Resolution option " << i << " clicked" << std::endl;
          selectedResolution = static_cast<int>(i);
          applyResolution(resolutions[i].width, resolutions[i].height);
          dropdownOpen = false;
          mousePressed = false;
          break;
        }
      }
    }
  }
  else if (backBounds.contains(mousePos))
  {
    selectedElement = 3;
    if (mousePressed)
    {
      std::cout << "[Settings] BACK clicked - returning to previous screen" << std::endl;
      game->goBackToPreviousScreen();
      mousePressed = false;
    }
  }
}

// Apply background music volume change
void SettingsScreen::applyVolume(float volume)
{
  float newVolumePercent = volume * 100.0f;
  game->setVolume(newVolumePercent);
}

// Apply sound effects volume change
void SettingsScreen::applySoundVolume(float volume)
{
  float newSoundVolumePercent = volume * 100.0f;
  game->setSoundVolume(newSoundVolumePercent);
  std::cout << "[Settings] Sound effects volume: " << newSoundVolumePercent << "%" << std::endl;
}

// Render the settings screen
void SettingsScreen::render(sf::RenderWindow &window, sf::Font &font)
{
  window.clear(sf::Color::White);

  const sf::Color COLOR_BLACK = Game::COLOR_BLACK;
  const sf::Color COLOR_OFF_WHITE = Game::COLOR_OFF_WHITE;

  sf::Vector2u windowSize = window.getSize();

  // Title
  float titleSize = windowSize.y * 0.078f;
  sf::Text titleText(font, "SETTINGS", static_cast<unsigned int>(titleSize));
  titleText.setFillColor(COLOR_BLACK);
  titleText.setStyle(sf::Text::Bold);

  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
  titleText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.10f});
  window.draw(titleText);

  // Settings panel
  float panelWidth = windowSize.x * 0.68f;
  float panelHeight = windowSize.y * 0.65f;
  float panelX = windowSize.x / 2.0f - panelWidth / 2.0f;
  float panelY = windowSize.y * 0.20f;

  sf::RectangleShape panel({panelWidth, panelHeight});
  panel.setPosition({panelX, panelY});
  panel.setFillColor(COLOR_OFF_WHITE);
  panel.setOutlineColor(COLOR_BLACK);
  panel.setOutlineThickness(4.0f);
  window.draw(panel);

  // Music Volume
  float labelSize = windowSize.y * 0.039f;
  sf::Text volumeLabel(font, "MUSIC VOLUME", static_cast<unsigned int>(labelSize));
  volumeLabel.setFillColor(COLOR_BLACK);
  volumeLabel.setStyle(sf::Text::Bold);
  volumeLabel.setPosition({panelX + panelWidth * 0.07f, panelY + panelHeight * 0.16f});
  window.draw(volumeLabel);

  float humorSize = windowSize.y * 0.020f;
  sf::Text volumeHumor(font, "Amplifying the bytecode symphony", static_cast<unsigned int>(humorSize));
  volumeHumor.setFillColor(COLOR_BLACK);
  volumeHumor.setStyle(sf::Text::Italic);
  volumeHumor.setPosition({panelX + panelWidth * 0.07f, panelY + panelHeight * 0.22f});
  window.draw(volumeHumor);

  // Music slider
  float sliderX = panelX + panelWidth * 0.36f;
  float sliderY = panelY + panelHeight * 0.26f;
  float sliderWidth = panelWidth * 0.43f;
  float sliderHeight = windowSize.y * 0.013f;

  sf::RectangleShape sliderTrack({sliderWidth, sliderHeight});
  sliderTrack.setPosition({sliderX, sliderY});
  sliderTrack.setFillColor(COLOR_BLACK);
  window.draw(sliderTrack);

  sf::RectangleShape sliderFill({volumePercent * sliderWidth, sliderHeight});
  sliderFill.setPosition({sliderX, sliderY});
  sliderFill.setFillColor(COLOR_BLACK);
  window.draw(sliderFill);

  float handleX = sliderX + volumePercent * sliderWidth;
  float handleRadius = windowSize.y * 0.016f;

  sf::CircleShape sliderHandle(handleRadius);
  sliderHandle.setOrigin({handleRadius, handleRadius});
  sliderHandle.setPosition({handleX, sliderY + sliderHeight / 2.0f});
  sliderHandle.setFillColor(selectedElement == 0 ? COLOR_BLACK : sf::Color::White);
  sliderHandle.setOutlineColor(COLOR_BLACK);
  sliderHandle.setOutlineThickness(2.0f);
  window.draw(sliderHandle);

  int volumePercentInt = static_cast<int>(volumePercent * 100);
  float percentSize = windowSize.y * 0.033f;
  sf::Text volumeValue(font, std::to_string(volumePercentInt) + "%", static_cast<unsigned int>(percentSize));
  volumeValue.setFillColor(COLOR_BLACK);
  volumeValue.setPosition({sliderX + sliderWidth + windowSize.x * 0.02f, sliderY - sliderHeight * 2.0f});
  window.draw(volumeValue);

  // Sound Effects Volume
  sf::Text soundLabel(font, "SOUND EFFECTS", static_cast<unsigned int>(labelSize));
  soundLabel.setFillColor(COLOR_BLACK);
  soundLabel.setStyle(sf::Text::Bold);
  soundLabel.setPosition({panelX + panelWidth * 0.07f, panelY + panelHeight * 0.31f});
  window.draw(soundLabel);

  sf::Text soundHumor(font, "Amplifying the click-clack symphony", static_cast<unsigned int>(humorSize));
  soundHumor.setFillColor(COLOR_BLACK);
  soundHumor.setStyle(sf::Text::Italic);
  soundHumor.setPosition({panelX + panelWidth * 0.07f, panelY + panelHeight * 0.37f});
  window.draw(soundHumor);

  float soundSliderY = panelY + panelHeight * 0.41f;

  sf::RectangleShape soundSliderTrack({sliderWidth, sliderHeight});
  soundSliderTrack.setPosition({sliderX, soundSliderY});
  soundSliderTrack.setFillColor(COLOR_BLACK);
  window.draw(soundSliderTrack);

  sf::RectangleShape soundSliderFill({soundVolumePercent * sliderWidth, sliderHeight});
  soundSliderFill.setPosition({sliderX, soundSliderY});
  soundSliderFill.setFillColor(COLOR_BLACK);
  window.draw(soundSliderFill);

  float soundHandleX = sliderX + soundVolumePercent * sliderWidth;

  sf::CircleShape soundSliderHandle(handleRadius);
  soundSliderHandle.setOrigin({handleRadius, handleRadius});
  soundSliderHandle.setPosition({soundHandleX, soundSliderY + sliderHeight / 2.0f});
  soundSliderHandle.setFillColor(selectedElement == 1 ? COLOR_BLACK : sf::Color::White);
  soundSliderHandle.setOutlineColor(COLOR_BLACK);
  soundSliderHandle.setOutlineThickness(2.0f);
  window.draw(soundSliderHandle);

  int soundVolumePercentInt = static_cast<int>(soundVolumePercent * 100);
  sf::Text soundVolumeValue(font, std::to_string(soundVolumePercentInt) + "%", static_cast<unsigned int>(percentSize));
  soundVolumeValue.setFillColor(COLOR_BLACK);
  soundVolumeValue.setPosition({sliderX + sliderWidth + windowSize.x * 0.02f, soundSliderY - sliderHeight * 2.0f});
  window.draw(soundVolumeValue);

  // Display Size
  sf::Text resolutionLabel(font, "DISPLAY SIZE", static_cast<unsigned int>(labelSize));
  resolutionLabel.setFillColor(COLOR_BLACK);
  resolutionLabel.setStyle(sf::Text::Bold);
  resolutionLabel.setPosition({panelX + panelWidth * 0.07f, panelY + panelHeight * 0.56f});
  window.draw(resolutionLabel);

  sf::Text displayHumor(font, "Expanding the viewport matrix", static_cast<unsigned int>(humorSize));
  displayHumor.setFillColor(COLOR_BLACK);
  displayHumor.setStyle(sf::Text::Italic);
  displayHumor.setPosition({panelX + panelWidth * 0.07f, panelY + panelHeight * 0.62f});
  window.draw(displayHumor);

  // Dropdown
  float dropdownX = panelX + panelWidth * 0.36f;
  float dropdownY = panelY + panelHeight * 0.56f;
  float dropdownWidth = panelWidth * 0.43f;
  float dropdownHeight = windowSize.y * 0.065f;

  sf::RectangleShape dropdown({dropdownWidth, dropdownHeight});
  dropdown.setPosition({dropdownX, dropdownY});
  dropdown.setFillColor(selectedElement == 2 ? COLOR_BLACK : sf::Color::White);
  dropdown.setOutlineColor(COLOR_BLACK);
  dropdown.setOutlineThickness(3.0f);
  window.draw(dropdown);

  float dropdownTextSize = windowSize.y * 0.033f;
  sf::Text currentRes(font, resolutions[selectedResolution].label, static_cast<unsigned int>(dropdownTextSize));
  currentRes.setFillColor(selectedElement == 2 ? sf::Color::White : COLOR_BLACK);
  currentRes.setPosition({dropdownX + dropdownWidth * 0.05f, dropdownY + dropdownHeight * 0.25f});
  window.draw(currentRes);

  float arrowSize = windowSize.y * 0.039f;
  sf::Text arrow(font, dropdownOpen ? "^" : "v", static_cast<unsigned int>(arrowSize));
  arrow.setFillColor(selectedElement == 2 ? sf::Color::White : COLOR_BLACK);
  arrow.setPosition({dropdownX + dropdownWidth - windowSize.x * 0.03f, dropdownY + dropdownHeight * 0.15f});
  window.draw(arrow);

  // Dropdown options
  if (dropdownOpen)
  {
    for (size_t i = 0; i < resolutions.size(); i++)
    {
      float optionHeight = windowSize.y * 0.052f;
      float optionY = dropdownY + dropdownHeight + (i * optionHeight);

      sf::RectangleShape option({dropdownWidth, optionHeight});
      option.setPosition({dropdownX, optionY});

      sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
      sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
      sf::FloatRect optionBounds({dropdownX, optionY}, {dropdownWidth, optionHeight});

      bool isHovered = optionBounds.contains(mousePos);
      bool isSelected = (i == static_cast<size_t>(selectedResolution));

      if (isSelected)
        option.setFillColor(COLOR_BLACK);
      else if (isHovered)
        option.setFillColor(COLOR_OFF_WHITE);
      else
        option.setFillColor(sf::Color::White);

      option.setOutlineColor(COLOR_BLACK);
      option.setOutlineThickness(2.0f);
      window.draw(option);

      float optionTextSize = windowSize.y * 0.029f;
      sf::Text optionText(font, resolutions[i].label, static_cast<unsigned int>(optionTextSize));
      optionText.setFillColor(isSelected ? sf::Color::White : COLOR_BLACK);
      optionText.setPosition({dropdownX + dropdownWidth * 0.05f, optionY + optionHeight * 0.20f});
      window.draw(optionText);
    }
  }

  // Back button
  float backButtonWidth = windowSize.x * 0.20f;
  float backButtonHeight = windowSize.y * 0.078f;
  float backButtonX = windowSize.x / 2.0f - backButtonWidth / 2.0f;
  float backButtonY = panelY + panelHeight + windowSize.y * 0.04f;

  sf::RectangleShape backButton({backButtonWidth, backButtonHeight});
  backButton.setPosition({backButtonX, backButtonY});
  backButton.setFillColor(selectedElement == 3 ? COLOR_BLACK : sf::Color::White);
  backButton.setOutlineColor(COLOR_BLACK);
  backButton.setOutlineThickness(4.0f);
  window.draw(backButton);

  float backTextSize = windowSize.y * 0.052f;
  sf::Text backText(font, "BACK", static_cast<unsigned int>(backTextSize));
  backText.setFillColor(selectedElement == 3 ? sf::Color::White : COLOR_BLACK);
  backText.setStyle(sf::Text::Bold);

  sf::FloatRect backTextBounds = backText.getLocalBounds();
  backText.setOrigin({backTextBounds.position.x + backTextBounds.size.x / 2.0f,
                      backTextBounds.position.y + backTextBounds.size.y / 2.0f});
  backText.setPosition({backButtonX + backButtonWidth / 2.0f, backButtonY + backButtonHeight / 2.0f});
  window.draw(backText);
}

// Apply resolution change
void SettingsScreen::applyResolution(unsigned int width, unsigned int height)
{
  std::cout << "[Settings] Applying resolution: " << width << "x" << height << std::endl;
  game->setResolution(width, height);
}

#include "ColourPatterns.h"

/**
 * @brief Constructs an instance of the ColourPatterns class.
 *
 * This constructor initializes an instance of the ColourPatterns class with the specified
 * pins for the red, green, and blue LEDs. It also sets initial values for various class
 * variables used in LED patterns.
 *
 * @param redPin The pin number connected to the red LED.
 * @param greenPin The pin number connected to the green LED.
 * @param bluePin The pin number connected to the blue LED.
 *
 * @note The constructor initializes the pins as OUTPUT and sets initial values for class
 *       variables such as `previousMillis`, `rainbowWay`, `threeWay`, `ledState`,
 *       `upDownFade`, `fadeSpeed`, and `interval`.
 * @note You can customize these variables to control the behavior of LED patterns.
 */
ColourPatterns::ColourPatterns(int redPin, int greenPin, int bluePin) : 
redLed(redPin), greenLed(greenPin), blueLed(bluePin), 
previousMillis(0),
rainbowWay(0),
threeWay(0),
ledState(LOW),
upDownFade(false),
fadeSpeed(500),
interval(100)
//todo: any other variables I missed? Are global variables better/ok instead? 

{
    // Constructor
    pinMode(redLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
    pinMode(blueLed, OUTPUT);
}

/**
 * @brief Runs a loading animation with RGB LEDs.
 *
 * This function runs a loading animation by sequentially lighting up the RGB LEDs in red,
 * green, and blue colors. It sets the red, green, and blue pins to different brightness
 * levels to create the loading effect.
 *
 * @note This function is designed for RGB LED control and creates a visual loading
 *       effect by cycling through the primary colors.
 * @note The loading animation consists of three phases: red, green, and blue, each
 *       lasting for 500 milliseconds.
 * @note You can customize the timing and colors of the loading animation as needed.
 */
void ColourPatterns::runLoading(){
  analogWrite(redLed, 255);
  analogWrite(greenLed, 0);
  analogWrite(blueLed, 0);
  delay(500);
  analogWrite(redLed, 0);
  analogWrite(greenLed, 255);
  analogWrite(blueLed, 0);
  delay(500);
  analogWrite(redLed, 0);
  analogWrite(greenLed, 0);
  analogWrite(blueLed, 255);
  delay(500);
}

/**
 * @brief Changes the color pattern of RGB LEDs based on the provided choice.
 *
 * This function allows you to change the color pattern of RGB LEDs based on a numeric
 * choice. It uses a switch statement to select a specific color pattern corresponding to
 * the choice and calls the respective color-setting function.
 *
 * @param choice An integer representing the chosen color pattern (0-13).
 *
 * @note The available color patterns are specified by the case values.
 * @note Default behavior is to turn off all LEDs.
 * @see Red() - Sets the LEDs to red.
 * @see Green() - Sets the LEDs to green.
 * @see Blue() - Sets the LEDs to blue.
 * @see Yellow() - Sets the LEDs to yellow.
 * @see Cyan() - Sets the LEDs to cyan.
 * @see Magenta() - Sets the LEDs to magenta.
 * @see White() - Sets the LEDs to white.
 */
void ColourPatterns::changeColours(int choice) {
    switch (choice) {
        case 0: Red(); break;
        case 1: Green(); break;
        case 2: Blue(); break;
        case 3: Cyan(); break;
        case 4: Magenta(); break;
        case 5: Yellow(); break;
        case 6: White(); break;
        case 7: Fade(); break;
        case 8: StrobePlus(); break;
        case 9: RGBStrobe(); break;
        case 10: Rainbow(); break;
        case 11: Halfstrobe(); break;
        case 12: GRStrobe(); break;
        case 13: BGStrobe(); break;
        default: Off(); break;
    }
}

// patterns here: 

/**
 * @brief Sets the RGB LEDs to the color red.
 * 
 * @note 0a Red
 */
void ColourPatterns::Red() {
  digitalWrite(redLed, HIGH);
  digitalWrite(blueLed, LOW);
  digitalWrite(greenLed, LOW);
}

/**
 * @brief Sets the RGB LEDs to the color green.
 * 
 * @note 1b Green
 */
void ColourPatterns::Green()
{
  digitalWrite(greenLed, HIGH);
  digitalWrite(blueLed, LOW);
  digitalWrite(redLed, LOW);
}

/**
 * @brief Sets the RGB LEDs to the color blue.
 * 
 * @note 2c Blue
 */
void ColourPatterns::Blue()
{
  digitalWrite(blueLed, HIGH);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
}


/**
 * @brief Sets the RGB LEDs to the color yellow (red + green).
 * 
 * @note 3d Yellow
 */
void ColourPatterns::Yellow()
{
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, HIGH);
  digitalWrite(blueLed, LOW);
}

/**
 * @brief Sets the RGB LEDs to the color cyan (green + blue).
 * 
 * @note 4e Cyan
 */
void ColourPatterns::Cyan()
{
  digitalWrite(blueLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
}

/**
 * @brief Sets the RGB LEDs to the color magenta (red + blue).
 * 
 * @note 5f Magenta
 */
void ColourPatterns::Magenta()
{
  digitalWrite(blueLed, HIGH);
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, LOW);
}

/**
 * @brief Sets the RGB LEDs to the color white (red + green + blue).
 * 
 * @note 6g White
 */
void ColourPatterns::White()
{
  digitalWrite(blueLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, HIGH);
}
// 7h Fade
/**
 * @brief Creates a fading color pattern effect on RGB LEDs.
 *
 * This function generates a fading color pattern on RGB LEDs by smoothly transitioning
 * between different colors. It adjusts the fade speed and direction, changing the color
 * every time a certain time interval has passed.
 *
 * @note 7h Fade
 * @note This function uses a software PWM-like approach to achieve the fading effect.
 * @note The fade speed and direction are controlled by the `fadeSpeed` variable and the
 *       `upDownFade` flag.
 * @note The color transitions include red, yellow, green, cyan, blue, and magenta.
 * @note The function updates the LEDs' colors based on the current time and fade speed.
 * @note The `rainbowWay` variable tracks the current color transition in the pattern.
 *
 * @see Red() - Sets the LEDs to red.
 * @see Yellow() - Sets the LEDs to yellow.
 * @see Green() - Sets the LEDs to green.
 * @see Cyan() - Sets the LEDs to cyan.
 * @see Blue() - Sets the LEDs to blue.
 * @see Magenta() - Sets the LEDs to magenta.
 */
void ColourPatterns::Fade()
{
  // delayMicroseconds fade software pwm was causing an issue so replaced with another Demo - might change this at some point.
  unsigned long currentMillis = millis();
  if (upDownFade)
  {
    if (fadeSpeed > 0) 
    {
      fadeSpeed--;

    } 
    else
    {
      upDownFade = false;
    }

  } 
  else
  {
    if (fadeSpeed < 5000)
    {
      fadeSpeed ++;
    }
    else
    {
      upDownFade = true;
    }
  }
  
  if (currentMillis - previousMillis >= fadeSpeed)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (rainbowWay == 0)
    {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      digitalWrite(blueLed, LOW);
    }
    else if (rainbowWay == 1)
    {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, HIGH);
      digitalWrite(blueLed, LOW);
    }
    else if (rainbowWay == 2)
    {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);
      digitalWrite(blueLed, LOW);
    }
    else if (rainbowWay == 3)
    {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);
      digitalWrite(blueLed, HIGH);
    }
    else if (rainbowWay == 4)
    {
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, LOW);
      digitalWrite(blueLed, HIGH);
    }
    else if (rainbowWay == 5)
    {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      digitalWrite(blueLed, HIGH);
    }
    rainbowWay++;
    if (rainbowWay > 5)
    {
      rainbowWay = 0;
    }
  }
  
}
// 8i Strobe+
void ColourPatterns::StrobePlus()
{
  // switch (strobePlusOptions)
  // {
  // case 0:
  //   oneSignal = redHEX;
  //   twoSignal = blueHEX;
  //   break;
  // case 1:
  //   oneSignal = blueHEX;
  //   twoSignal = greenHEX;
  //   break;
  // case 2:
  //   oneSignal = greenHEX;
  //   twoSignal = redHEX;
  //   break;
  // case 3:
  //   oneSignal = cyanHEX;
  //   twoSignal = magentaHEX;
  //   break;
  // case 4:
  //   oneSignal = magentaHEX;
  //   twoSignal = yellowHEX;
  //   break;
  // case 5:
  //   oneSignal = yellowHEX;
  //   twoSignal = cyanHEX;
  //   break;
  // default:
  //   // nothing
  //   break;
  // }
  // strobePlusOptions++;
  // if (strobePlusOptions > strobePlusOptionsMax)
  // {
  //   strobePlusOptions = 0;
  // }
  // flashThreeWay = true;
}

/**
 * @brief Generates a strobe-like color pattern effect on RGB LEDs.
 *
 * This function creates a strobe-like color pattern effect on RGB LEDs. However, the
 * implementation currently appears to be commented out, and the function's code does
 * not contain any active logic.
 *
 * @note 9j RGBStrobe
 * @note The implementation is currently commented out, and there is no active code
 *       within this function.
 * @note It seems that the function was intended to control color transitions based on
 *       the `strobePlusOptions` variable, but this functionality is commented out.
 * @note If you intend to use this function, you may need to uncomment and modify the
 *       logic to achieve the desired strobe-like effect.
 */
void ColourPatterns::RGBStrobe()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    if (threeWay == 0)
    {
      Red();
    }
    else if (threeWay == 1)
    {
      Green();
    }
    else if (threeWay == 2)
    {
      Blue();
    }
    threeWay++;
    if (threeWay > 2)
    {
      threeWay = 0;
    }
  }
}

/**
 * @brief Generates a rainbow color pattern effect on RGB LEDs.
 *
 * This function creates a rainbow color pattern effect on RGB LEDs by sequentially
 * transitioning between red, green, blue, cyan, yellow, and magenta colors. It updates
 * the color pattern at regular intervals based on the specified `interval`.
 *
 * @param interval The time interval (in milliseconds) between color transitions.
 *
 * @note 10k Rainbow
 * @note This function is designed to create a continuous rainbow-like color pattern.
 * @note The color transitions include red, green, blue, cyan, yellow, and magenta.
 * @note The `interval` parameter controls the time between color transitions.
 * @note The `rainbowWay` variable tracks the current color in the rainbow sequence.
 *
 * @see Red() - Sets the LEDs to red.
 * @see Green() - Sets the LEDs to green.
 * @see Blue() - Sets the LEDs to blue.
 * @see Cyan() - Sets the LEDs to cyan.
 * @see Yellow() - Sets the LEDs to yellow.
 * @see Magenta() - Sets the LEDs to magenta.
 */
void ColourPatterns::Rainbow()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // Serial.println("interval elapsed");
    previousMillis = currentMillis;
    if (rainbowWay == 0)
    {
      Red();
    }
    else if (rainbowWay == 1)
    {
      Green();
    }
    else if (rainbowWay == 2)
    {
      Blue();
    }
    else if (rainbowWay == 3)
    {
      Cyan();
    }
    else if (rainbowWay == 4)
    {
      Yellow();
    }
    else if (rainbowWay == 5)
    {
      Magenta();
    }
    rainbowWay++;
    if (rainbowWay > 5)
    {
      rainbowWay = 0;
    }
  }
}

/**
 * @brief Generates a half-strobe color pattern effect on RGB LEDs.
 *
 * This function creates a half-strobe color pattern effect on RGB LEDs by toggling
 * between red and blue colors at regular intervals. It alternates between turning
 * on and off the LEDs to create a flashing effect.
 *
 * @note 11L Halfstrobe
 * @note This function generates a two-color strobe effect between red and blue.
 * @note The `interval` parameter controls the time between color toggles.
 * @note The `ledState` variable keeps track of the current LED state (ON/OFF).
 *
 * @see Red() - Sets the LEDs to red.
 * @see Blue() - Sets the LEDs to blue.
 */
void ColourPatterns::Halfstrobe()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
    {
      Red();
      ledState = HIGH;
    }
    else
    {
      Blue();
      ledState = LOW;
    }
  }
}

/**
 * @brief Generates a blue-green strobe color pattern effect on RGB LEDs.
 *
 * This function creates a blue-green strobe color pattern effect on RGB LEDs by toggling
 * between blue and green colors at regular intervals. It alternates between turning
 * on and off the LEDs to create a flashing effect.
 *
 * @note 12m BGStrobe
 * @note This function generates a two-color strobe effect between blue and green.
 * @note The `interval` parameter controls the time between color toggles.
 * @note The `ledState` variable keeps track of the current LED state (ON/OFF).
 *
 * @see Blue() - Sets the LEDs to blue.
 * @see Green() - Sets the LEDs to green.
 */
void ColourPatterns::BGStrobe()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
    {
      Blue();
      ledState = HIGH;
    }
    else
    {
      Green();
      ledState = LOW;
    }
  }
}

/**
 * @brief Generates a green-red strobe color pattern effect on RGB LEDs.
 *
 * This function creates a green-red strobe color pattern effect on RGB LEDs by toggling
 * between green and red colors at regular intervals. It alternates between turning
 * on and off the LEDs to create a flashing effect.
 *
 * @note 13n GRStrobe
 * @note This function generates a two-color strobe effect between green and red.
 * @note The `interval` parameter controls the time between color toggles.
 * @note The `ledState` variable keeps track of the current LED state (ON/OFF).
 *
 * @see Green() - Sets the LEDs to green.
 * @see Red() - Sets the LEDs to red.
 */
void ColourPatterns::GRStrobe()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
    {
      Green();
      ledState = HIGH;
    }
    else
    {
      Red();
      ledState = LOW;
    }
  }
}

/**
 * @brief Turns off all RGB LEDs.
 *
 * This function turns off all RGB LEDs by setting their respective pins to a LOW state.
 * 
 * @note 14o Off
 */
void ColourPatterns::Off()
{
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(redLed, LOW);
}
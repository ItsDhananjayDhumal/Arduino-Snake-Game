#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

//OLED Display setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define MAX_LEN 128
#define GIRTH 2 //   ### MUST BE A POWER OF 2 (including 1)
#define SPEED 15 // speed of iteration. delay is SPEED * GIRTH

#define JOYSTICKX A0
#define JOYSTICKY A1

short joystick_x;
short joystick_y;

uint8_t a_xcor;
uint8_t a_ycor;

uint16_t snake_len = 5;
short x_speed = GIRTH; // cannot be uint8_t because speed can have negative value
short y_speed = 0;

uint8_t score = 0;

uint8_t x_cor[MAX_LEN] = {0};
uint8_t y_cor[MAX_LEN] = {0};

#define OLED_SCL A5
#define OLED_SDA A4
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void GAME_OVER() {
    delay(1500);
    display.clearDisplay();
    display.setCursor(10, 25);
    display.setTextColor(1);
    display.setTextSize(2);
    display.print("GAME OVER");
    display.setTextSize(1);
    display.print("                                                  Score: ");
    display.print(score);
    display.display();
    for(;;){}
}

void RandomApple(){
    a_xcor = (GIRTH * random((20/GIRTH), ((SCREEN_WIDTH - 20)/GIRTH)));
    a_ycor = (GIRTH * random((20/GIRTH), ((SCREEN_HEIGHT - 20)/GIRTH)));
}

void setup() {
    Serial.begin(9600);
    display.clearDisplay();
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    randomSeed(analogRead(A4));

    RandomApple();

  // init snake
    for (short i = 0; i < snake_len; i++) {
        x_cor[i] = 64 - (i * GIRTH);
        y_cor[i] = 32;
    }
}

void loop() {
    display.clearDisplay();

    joystick_x = analogRead(JOYSTICKX);
    joystick_y = analogRead(JOYSTICKY);

    if (joystick_x < 400 && x_speed != -GIRTH) {
        x_speed = GIRTH;
        y_speed = 0;
    }
    else if (joystick_x > 600 && x_speed != GIRTH) {
        x_speed = -GIRTH;
        y_speed = 0;
    }
    else if (joystick_y < 400 && y_speed != GIRTH) {
        x_speed = 0;
        y_speed = -GIRTH;
    }
    else if (joystick_y > 600 && y_speed != -GIRTH) {
        x_speed = 0;
        y_speed = GIRTH;
    }

    for (short i = 1; i < snake_len; i++) {
        x_cor[snake_len - i] = x_cor[snake_len - i - 1];
        y_cor[snake_len - i] = y_cor[snake_len - i - 1];
    }

    x_cor[0] += x_speed;
    y_cor[0] += y_speed;

    if (x_cor[0] < 0 || x_cor[0] > (128 - GIRTH) || y_cor[0] < 0 || y_cor[0] > (64 - GIRTH)) {
        GAME_OVER();
    }

    for (short i = 1; i < snake_len; i++) {
        if (x_cor[0] == x_cor[i] && y_cor[0] == y_cor[i]) {
            GAME_OVER();
            // return;
        }
    }

    if (x_cor[0] == a_xcor && y_cor[0] == a_ycor) {
        score++;
        RandomApple();
        x_cor[snake_len] = x_cor[snake_len - 1];
        y_cor[snake_len] = y_cor[snake_len - 1];
        x_cor[snake_len + 1] = x_cor[snake_len - 1];
        y_cor[snake_len + 1] = y_cor[snake_len - 1];

        snake_len += 2;
    }

    Serial.println(snake_len);

    display.drawRoundRect(a_xcor, a_ycor,GIRTH, GIRTH, (GIRTH/2), 1);
    for (short i = 0; i < snake_len; i++) {
        display.fillRect(x_cor[i], y_cor[i], GIRTH, GIRTH, 1);
    }    
    display.display();
    delay(SPEED*GIRTH);
}
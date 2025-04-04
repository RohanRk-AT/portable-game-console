#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define PIPE_WIDTH 7
#define GAP_HEIGHT 20
#define GRAVITY 1
#define JUMP_STRENGTH -4
#define PLAYER_SIZE 6
#define NUM_PIPES 2
#define BUTTON_PIN 2
#define DEBOUNCE_DELAY 100

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int playerY = SCREEN_HEIGHT / 2;
float velocity = 0;
int pipeX[NUM_PIPES] = {SCREEN_WIDTH, SCREEN_WIDTH + SCREEN_WIDTH / 2};
int pipeGapY[NUM_PIPES] = {SCREEN_HEIGHT / 4, SCREEN_HEIGHT / 2};
int score = 0;
bool gameOver = false;
bool gameStarted = false;
bool inMenu = true;
unsigned long lastButtonPress = 0;

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.clearDisplay();
    display.display();
    showMenu();
}

void loop() {

  
    if (inMenu) {
        if (millis() - lastButtonPress > DEBOUNCE_DELAY && digitalRead(BUTTON_PIN) == LOW) {
            lastButtonPress = millis();
            inMenu = false;
            resetGame();
        }
        return;
    }

    if (gameOver) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 10);
        display.print("Game Over!");
        display.setCursor(20, 20);
        display.print("Score: ");
        display.print(score);
        display.display();
        delay(2000);
        inMenu = true;
        showMenu();
        return;
    }

    if (millis() - lastButtonPress > DEBOUNCE_DELAY && digitalRead(BUTTON_PIN) == LOW) {
        velocity = JUMP_STRENGTH;
        gameStarted = true;
        lastButtonPress = millis();
    }

    if (gameStarted) {
        velocity += GRAVITY;
        playerY += velocity;
    }

    if (playerY <= 0) {
        playerY = 0;
        velocity = 0;
    }
    if (playerY + PLAYER_SIZE >= SCREEN_HEIGHT) {
        playerY = SCREEN_HEIGHT - PLAYER_SIZE;
        velocity = 0;
    }

    for (int i = 0; i < NUM_PIPES; i++) {
        pipeX[i] -= 2;
        if (pipeX[i] < -PIPE_WIDTH) {
            pipeX[i] = SCREEN_WIDTH;
            pipeGapY[i] = random(5, SCREEN_HEIGHT - GAP_HEIGHT - 5);
            score++;
        }
        if (8 + PLAYER_SIZE / 2 > pipeX[i] && 8 - PLAYER_SIZE / 2 < pipeX[i] + PIPE_WIDTH) {
            if (playerY < pipeGapY[i] || playerY + PLAYER_SIZE > pipeGapY[i] + GAP_HEIGHT) {
                gameOver = true;
            }
        }
    }

    display.clearDisplay();
    display.fillCircle(8, playerY + PLAYER_SIZE / 2, PLAYER_SIZE / 2, SSD1306_WHITE);

    for (int i = 0; i < NUM_PIPES; i++) {
        display.fillRect(pipeX[i], 0, PIPE_WIDTH, pipeGapY[i], SSD1306_WHITE);
        display.fillRect(pipeX[i], pipeGapY[i] + GAP_HEIGHT, PIPE_WIDTH, SCREEN_HEIGHT - pipeGapY[i] - GAP_HEIGHT, SSD1306_WHITE);
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(100, 0);
    display.print(score);
    display.display();
    delay(30);
}

void resetGame() {
    playerY = SCREEN_HEIGHT / 2;
    velocity = 0;
    score = 0;
    gameOver = false;
    gameStarted = false;
    pipeX[0] = SCREEN_WIDTH;
    pipeX[1] = SCREEN_WIDTH + SCREEN_WIDTH / 2;
    pipeGapY[0] = SCREEN_HEIGHT / 4;
    pipeGapY[1] = SCREEN_HEIGHT / 2;
}

void showMenu() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 5);
    display.print("[BOUNCE]");
    display.setTextSize(1);
    display.setCursor(50, 20);
    display.print("Start");
    display.display();
}

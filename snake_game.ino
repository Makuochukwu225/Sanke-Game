#include <LedControl.h>

// LED Matrix (DIN=11, CLK=13, CS=10)
LedControl lc = LedControl(11, 13, 10, 1);

// Joystick pins
const int xPin = A0;
const int yPin = A1;

// Buzzer pin
const int buzzerPin = 3;

// Snake representation
struct Point {
  int x;
  int y;
};

Point snake[64];
int snakeLength = 3;
int dx = 1, dy = 0;

Point food;

unsigned long lastMoveTime = 0;
const unsigned long moveInterval = 300; // ms

void setup() {
  // Matrix init
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // Joystick + buzzer
  pinMode(buzzerPin, OUTPUT);
  randomSeed(analogRead(A5)); // entropy for food

  // Start snake
  snake[0] = {4, 4};
  snake[1] = {3, 4};
  snake[2] = {2, 4};

  generateFood();
  playStartTone();
}

void loop() {
  readJoystick();

  if (millis() - lastMoveTime >= moveInterval) {
    moveSnake();
    drawSnake();
    lastMoveTime = millis();
  }
}

// === INPUT HANDLER ===
void readJoystick() {
  int x = analogRead(xPin);
  int y = analogRead(yPin);

  // prevent reversing direction
  if (x < 400 && dx == 0) { dx = -1; dy = 0; }
  else if (x > 600 && dx == 0) { dx = 1; dy = 0; }
  else if (y < 400 && dy == 0) { dx = 0; dy = -1; }
  else if (y > 600 && dy == 0) { dx = 0; dy = 1; }
}

// === SNAKE LOGIC ===
void moveSnake() {
  Point newHead = { snake[0].x + dx, snake[0].y + dy };

  // wrap-around
  newHead.x = (newHead.x + 8) % 8;
  newHead.y = (newHead.y + 8) % 8;

  // collision with self
  for (int i = 0; i < snakeLength; i++) {
    if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
      playGameOverTone();
      resetGame();
      return;
    }
  }

  // shift snake
  for (int i = snakeLength; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0] = newHead;

  // food eaten
  if (newHead.x == food.x && newHead.y == food.y) {
    snakeLength++;
    generateFood();
    playFoodTone();
  }
}

// === DISPLAY ===
void drawSnake() {
  lc.clearDisplay(0);
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snake[i].y, snake[i].x, true);
  }
  lc.setLed(0, food.y, food.x, true);
}

// === FOOD ===
void generateFood() {
  while (true) {
    food.x = random(8);
    food.y = random(8);
    bool onSnake = false;
    for (int i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        onSnake = true;
        break;
      }
    }
    if (!onSnake) break;
  }
}

// === RESET ===
void resetGame() {
  snakeLength = 3;
  snake[0] = {4, 4};
  snake[1] = {3, 4};
  snake[2] = {2, 4};
  dx = 1; dy = 0;
  generateFood();
}

// === SOUND EFFECTS ===
void playStartTone() {
  tone(buzzerPin, 1000, 150);
  delay(200);
  tone(buzzerPin, 1500, 150);
  delay(200);
  noTone(buzzerPin);
}

void playFoodTone() {
  tone(buzzerPin, 900, 100);
  delay(120);
  noTone(buzzerPin);
}

void playGameOverTone() {
  tone(buzzerPin, 300, 600);
  delay(600);
  noTone(buzzerPin);
}

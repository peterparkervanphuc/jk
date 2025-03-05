#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;
const int BLOCK_SIZE = 20;
const int NUM_BLOCKS_X = WIDTH / BLOCK_SIZE;
const int NUM_BLOCKS_Y = HEIGHT / BLOCK_SIZE;

// Định nghĩa các màu
SDL_Color Green = {0, 255, 0};
SDL_Color Red = {255, 0, 0};
SDL_Color Blue = {0, 0, 255};
SDL_Color Yellow = {255, 255, 0};
SDL_Color White = {255, 255, 255};

// Các hướng di chuyển của rắn
enum Direction { UP, DOWN, LEFT, RIGHT };

// Cấu trúc cho rắn
struct SnakeSegment {
    int x, y;
};

// Hàm vẽ hình vuông với màu tùy chỉnh
void DrawRectangle(SDL_Renderer* renderer, int x, int y, int width, int height, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_Rect rect = { x, y, width, height };
    SDL_RenderFillRect(renderer, &rect);
}

// Hàm vẽ hệ thống tính điểm
void DrawScore(SDL_Renderer* renderer, TTF_Font* font, int score) {
    SDL_Color color = { 255, 255, 255 }; // Màu trắng cho điểm số
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, ("Score: " + std::to_string(score)).c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = { 10, 10, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

// Hàm vẽ rắn
void DrawSnake(SDL_Renderer* renderer, const std::deque<SnakeSegment>& snake) {
    for (const auto& segment : snake) {
        DrawRectangle(renderer, segment.x * BLOCK_SIZE, segment.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, Green);
    }
}

// Hàm vẽ mồi
void DrawFood(SDL_Renderer* renderer, int foodX, int foodY) {
    DrawRectangle(renderer, foodX * BLOCK_SIZE, foodY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, Red);
}

// Hàm kiểm tra va chạm
bool CheckCollision(const std::deque<SnakeSegment>& snake, int x, int y) {
    if (x < 1 || y < 1 || x >= NUM_BLOCKS_X - 1 || y >= NUM_BLOCKS_Y - 1) {
        return true; // Chạm viền
    }
    for (const auto& segment : snake) {
        if (segment.x == x && segment.y == y) {
            return true; // Chạm vào thân
        }
    }
    return false;
}

// Hàm cập nhật vị trí mồi ngẫu nhiên
void SpawnFood(int& foodX, int& foodY, const std::deque<SnakeSegment>& snake) {
    bool valid = false;
    while (!valid) {
        foodX = rand() % NUM_BLOCKS_X;
        foodY = rand() % NUM_BLOCKS_Y;
        valid = true;
        for (const auto& segment : snake) {
            if (segment.x == foodX && segment.y == foodY) {
                valid = false;
                break;
            }
        }
    }
}

void ResetGame(std::deque<SnakeSegment>& snake, Direction& dir, int& score, int& foodX, int& foodY) {
    snake = {{5, 5}, {4, 5}, {3, 5}};  // Khởi tạo lại vị trí rắn
    dir = RIGHT;  // Đặt lại hướng di chuyển
    score = 0;    // Đặt lại điểm số
    SpawnFood(foodX, foodY, snake);  // Đặt lại mồi
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game Rắn SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("Orange Fizz.ttf", 24);
    if (!font) {
        std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Khởi tạo rắn
    std::deque<SnakeSegment> snake = {{5, 5}, {4, 5}, {3, 5}};
    Direction dir = RIGHT;
    int foodX, foodY;
    SpawnFood(foodX, foodY, snake);

    int score = 0;
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        SDL_RenderClear(renderer);

        // Vẽ nền xanh nước biển
        DrawRectangle(renderer, 0, 0, WIDTH, HEIGHT, Blue);

        // Vẽ viền vàng nhỏ
        DrawRectangle(renderer, 0, 0, WIDTH, BLOCK_SIZE, Yellow);
        DrawRectangle(renderer, 0, HEIGHT - BLOCK_SIZE, WIDTH, BLOCK_SIZE, Yellow);
        DrawRectangle(renderer, 0, 0, BLOCK_SIZE, HEIGHT, Yellow);
        DrawRectangle(renderer, WIDTH - BLOCK_SIZE, 0, BLOCK_SIZE, HEIGHT, Yellow);

        // Vẽ rắn và mồi
        DrawSnake(renderer, snake);
        DrawFood(renderer, foodX, foodY);

        // Vẽ điểm số
        DrawScore(renderer, font, score);

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);

        // Xử lý sự kiện
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP && dir != DOWN) {
                    dir = UP;
                } else if (event.key.keysym.sym == SDLK_DOWN && dir != UP) {
                    dir = DOWN;
                } else if (event.key.keysym.sym == SDLK_LEFT && dir != RIGHT) {
                    dir = LEFT;
                } else if (event.key.keysym.sym == SDLK_RIGHT && dir != LEFT) {
                    dir = RIGHT;
                }
            }
        }

        // Di chuyển rắn
        SnakeSegment newHead = snake.front();
        if (dir == UP) {
            newHead.y--;
        } else if (dir == DOWN) {
            newHead.y++;
        } else if (dir == LEFT) {
            newHead.x--;
        } else if (dir == RIGHT) {
            newHead.x++;
        }

        if (CheckCollision(snake, newHead.x, newHead.y)) {
            ResetGame(snake, dir, score, foodX, foodY);  // Hồi sinh và reset game
        }

        // Kiểm tra ăn mồi
        if (newHead.x == foodX && newHead.y == foodY) {
            snake.push_front(newHead);
            score++;
            SpawnFood(foodX, foodY, snake);
        } else {
            snake.push_front(newHead);
            snake.pop_back();
        }

        SDL_Delay(100); // Điều chỉnh tốc độ game
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

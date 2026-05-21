/*
 * TANK GAME - Battle City Style
 * C++ + Raylib
 * Compile with: g++ tank_game.cpp -o tank -lraylib -lopengl32 -lgdi32 -lwinmm
 * Or setup in Code::Blocks with Raylib library linked
 */

#include <raylib.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// ============================================================================
// KONSTANTA & ENUM
// ============================================================================
const int TILE_SIZE = 32;
const int MAP_WIDTH = 26;  // 832px
const int MAP_HEIGHT = 26; // 832px
const int SCREEN_WIDTH = MAP_WIDTH * TILE_SIZE;
const int SCREEN_HEIGHT = MAP_HEIGHT * TILE_SIZE;
const int FPS = 60;

enum TileType { TILE_EMPTY = 0, TILE_BRICK = 1, TILE_STEEL = 2, TILE_WATER = 3, TILE_BASE = 4 };
enum Direction { DIR_UP = 0, DIR_RIGHT = 1, DIR_DOWN = 2, DIR_LEFT = 3 };
enum PowerUpType { POWER_GUN = 0, POWER_HP = 1, POWER_SPEED = 2, POWER_ARMOR = 3 };
enum WaveShape { WAVE_SINE = 0, WAVE_SQUARE = 1, WAVE_SAWTOOTH = 2 };

const int MAX_PLAYER_LEVEL = 5;

// ============================================================================
// DATA MAP (26x26) - 1=Dinding Hancur, 2=Baja, 3=Air, 4=Base, 0=Kosong
// ============================================================================
// Map sederhana bergaya retro
int tileMap[MAP_HEIGHT][MAP_WIDTH] = {
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,2,2,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,2,2,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,2,2,2,2,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,2,2,2,2,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,2,2,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,1,1,0,1,1,0,0,0,2,2,0,0,0,1,1,0,1,1,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,2},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
};

// ============================================================================
// STRUKTUR DATA
// ============================================================================
struct Bullet {
    Vector2 pos;
    Direction dir;
    float speed;
    bool active;
    bool isPlayer; // true = peluru player
    float radius;
    int damage;
};

struct PowerUp {
    Vector2 pos;
    PowerUpType type;
    bool active;
    float timer;
    int size;
};

struct Tank {
    Vector2 pos;
    Direction dir;
    float speed;
    int hp;
    bool active;
    bool isPlayer;
    float shootCooldown;
    float shootTimer;
    Color color;
    float moveTimer;
    float changeDirTimer;
    float radius;
    int size;
    int level;
    int shotCount;
    int maxBullets;
    int bulletDamage;
    int maxHp;
};

// ============================================================================
// VARIABEL GLOBAL
// ============================================================================
Tank player;
vector<Tank> enemies;
vector<Bullet> bullets;
vector<PowerUp> powerUps;

Sound sndShoot;
Sound sndExplosion;
Sound sndHit;

int score = 0;
int enemiesToSpawn = 6;
int enemiesAlive = 0;
bool gameOver = false;
bool playerWon = false;
float spawnTimer = 0.0f;
float spawnDelay = 2.0f;

// ============================================================================
// FUNGSI UTILITAS
// ============================================================================
Wave GenToneWave(WaveShape shape, float frequency, float duration, int sampleRate = 44100) {
    int sampleCount = (int)(duration * sampleRate);
    short *samples = (short *)MemAlloc(sampleCount * sizeof(short));

    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / (float)sampleRate;
        float phase = frequency * t;
        phase = phase - floorf(phase);

        float value = 0.0f;
        switch (shape) {
            case WAVE_SINE:     value = sinf(2.0f * PI * frequency * t); break;
            case WAVE_SQUARE:   value = phase < 0.5f ? 1.0f : -1.0f; break;
            case WAVE_SAWTOOTH: value = 2.0f * phase - 1.0f; break;
        }

        int fadeSamples = sampleRate / 200;
        float envelope = 1.0f;
        if (i < fadeSamples) envelope = (float)i / (float)fadeSamples;
        else if (i > sampleCount - fadeSamples) envelope = (float)(sampleCount - i) / (float)fadeSamples;

        samples[i] = (short)(value * envelope * 28000);
    }

    Wave wave = { 0 };
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = samples;
    return wave;
}

Rectangle GetTankRect(const Tank& t) {
    return { t.pos.x - t.size/2.0f, t.pos.y - t.size/2.0f, (float)t.size, (float)t.size };
}

Rectangle GetTileRect(int tx, int ty) {
    return { (float)(tx * TILE_SIZE), (float)(ty * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
}

bool RectIntersect(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

Vector2 DirToVector(Direction d) {
    switch(d) {
        case DIR_UP:    return {0, -1};
        case DIR_DOWN:  return {0, 1};
        case DIR_LEFT:  return {-1, 0};
        case DIR_RIGHT: return {1, 0};
    }
    return {0,0};
}

bool IsTileBlocked(int tx, int ty) {
    if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT) return true;
    int t = tileMap[ty][tx];
    return (t == TILE_BRICK || t == TILE_STEEL || t == TILE_WATER || t == TILE_BASE);
}

bool TankCollidesWithMap(const Tank& t, float newX, float newY) {
    Rectangle tr = { newX - t.size/2.0f, newY - t.size/2.0f, (float)t.size, (float)t.size };
    // Cek tile di sekitar tank
    int startX = (int)(tr.x / TILE_SIZE) - 1;
    int startY = (int)(tr.y / TILE_SIZE) - 1;
    int endX = (int)((tr.x + tr.width) / TILE_SIZE) + 1;
    int endY = (int)((tr.y + tr.height) / TILE_SIZE) + 1;

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) continue;
            if (tileMap[y][x] == TILE_BRICK || tileMap[y][x] == TILE_STEEL || tileMap[y][x] == TILE_WATER) {
                Rectangle tileR = GetTileRect(x, y);
                if (RectIntersect(tr, tileR)) return true;
            }
        }
    }
    return false;
}

bool TanksCollide(const Tank& a, const Tank& b) {
    return RectIntersect(GetTankRect(a), GetTankRect(b));
}

// ============================================================================
// INISIALISASI
// ============================================================================
void InitTank(Tank& t, float x, float y, bool isPlayer) {
    t.pos = {x, y};
    t.dir = DIR_UP;
    t.speed = isPlayer ? 120.0f : 60.0f; // Musuh lebih lambat
    t.maxHp = isPlayer ? 3 : 1;
    t.hp = t.maxHp;
    t.active = true;
    t.isPlayer = isPlayer;
    t.shootCooldown = isPlayer ? 0.35f : 1.2f;
    t.shootTimer = 0.0f;
    t.color = isPlayer ? DARKGREEN : RED;
    t.moveTimer = 0.0f;
    t.changeDirTimer = 0.0f;
    t.radius = 12.0f;
    t.size = 28;
    t.level = isPlayer ? 1 : 1;
    t.shotCount = 1;
    t.maxBullets = isPlayer ? 2 : 1;
    t.bulletDamage = 1;
}

void SpawnEnemy() {
    if (enemiesToSpawn <= 0) return;
    // Cari posisi spawn acak di bagian atas map
    int attempts = 0;
    while (attempts < 50) {
        int tx = 2 + rand() % (MAP_WIDTH - 4);
        int ty = 2 + rand() % 8; // Bagian atas
        float x = tx * TILE_SIZE + TILE_SIZE/2.0f;
        float y = ty * TILE_SIZE + TILE_SIZE/2.0f;

        Tank e;
        InitTank(e, x, y, false);
        // Cek tabrakan dengan map
        if (!TankCollidesWithMap(e, x, y)) {
            // Cek tabrakan dengan tank lain
            bool collides = false;
            if (TanksCollide(e, player)) collides = true;
            for (auto& en : enemies) {
                if (en.active && TanksCollide(e, en)) collides = true;
            }
            if (!collides) {
                enemies.push_back(e);
                enemiesToSpawn--;
                enemiesAlive++;
                break;
            }
        }
        attempts++;
    }
}

void InitGame() {
    srand((unsigned)time(nullptr));

    // Reset map
    // (Jika ingin reload map asli, bisa disimpan di array terpisah)

    // Init player di bawah tengah
    InitTank(player, (MAP_WIDTH/2) * TILE_SIZE + TILE_SIZE/2.0f, (MAP_HEIGHT - 4) * TILE_SIZE + TILE_SIZE/2.0f, true);

    enemies.clear();
    bullets.clear();
    powerUps.clear();
    score = 0;
    enemiesToSpawn = 12; // total musuh per round
    enemiesAlive = 0;
    gameOver = false;
    playerWon = false;
    spawnTimer = spawnDelay;
}

// ============================================================================
// LOGIKA GAME
// ============================================================================
void Shoot(Tank& t) {
    if (!t.active) return;
    if (t.shootTimer > 0) return;

    // Batasi jumlah peluru player yang aktif agar upgrade terasa seperti equipment.
    if (t.isPlayer) {
        int activePlayerBullets = 0;
        for (auto& existing : bullets) {
            if (existing.active && existing.isPlayer) activePlayerBullets++;
        }
        if (activePlayerBullets >= t.maxBullets) return;
    }

    Vector2 d = DirToVector(t.dir);
    Vector2 perp = { -d.y, d.x }; // arah samping untuk double/triple shot

    int shots = t.isPlayer ? t.shotCount : 1;
    if (shots < 1) shots = 1;
    if (shots > 3) shots = 3;

    for (int i = 0; i < shots; i++) {
        float offset = 0.0f;
        if (shots == 2) offset = (i == 0) ? -6.0f : 6.0f;
        if (shots == 3) offset = (i - 1) * 7.0f;

        Bullet b;
        b.pos = {
            t.pos.x + d.x * (t.size/2.0f + 10) + perp.x * offset,
            t.pos.y + d.y * (t.size/2.0f + 10) + perp.y * offset
        };
        b.dir = t.dir;
        b.speed = t.isPlayer ? 280.0f + (t.level * 12.0f) : 250.0f;
        b.active = true;
        b.isPlayer = t.isPlayer;
        b.radius = t.isPlayer ? 4.0f + t.level * 0.35f : 4.0f;
        b.damage = t.bulletDamage;
        bullets.push_back(b);
    }

    t.shootTimer = t.shootCooldown;

    if (IsAudioDeviceReady()) {
        PlaySound(sndShoot);
    }
}


void SpawnPowerUp(Vector2 pos) {
    // Tidak semua musuh menjatuhkan item, supaya tetap terasa bernilai.
    if ((rand() % 100) >= 40) return;

    PowerUp p;
    p.pos = pos;
    p.type = (PowerUpType)(rand() % 4);
    p.active = true;
    p.timer = 12.0f; // hilang setelah 12 detik
    p.size = 24;
    powerUps.push_back(p);
}

void UpgradePlayerGun() {
    if (player.level < MAX_PLAYER_LEVEL) player.level++;

    // Level equipment senjata:
    // Lv1: 1 peluru, Lv2: 2 peluru paralel, Lv3+: 3 peluru paralel + damage/cooldown lebih baik.
    if (player.level >= 2) player.shotCount = 2;
    if (player.level >= 3) player.shotCount = 3;

    player.maxBullets = 2 + player.level;              // semakin tinggi, semakin banyak peluru aktif
    player.bulletDamage = (player.level >= 4) ? 2 : 1; // level 4+ bisa menghancurkan musuh kuat lebih cepat
    player.shootCooldown = 0.35f - player.level * 0.035f;
    if (player.shootCooldown < 0.16f) player.shootCooldown = 0.16f;
}

void ApplyPowerUp(PowerUp& p) {
    if (!p.active) return;

    switch (p.type) {
        case POWER_GUN:
            UpgradePlayerGun();
            score += 50;
            break;
        case POWER_HP:
            player.hp++;
            if (player.hp > player.maxHp) player.maxHp = player.hp;
            if (player.hp > 8) player.hp = 8;
            if (player.maxHp > 8) player.maxHp = 8;
            score += 25;
            break;
        case POWER_SPEED:
            player.speed += 15.0f;
            if (player.speed > 190.0f) player.speed = 190.0f;
            score += 25;
            break;
        case POWER_ARMOR:
            player.maxHp++;
            if (player.maxHp > 8) player.maxHp = 8;
            player.hp = player.maxHp;
            score += 35;
            break;
    }

    p.active = false;
    if (IsAudioDeviceReady()) PlaySound(sndHit);
}

void UpdatePowerUps(float dt) {
    Rectangle pr = GetTankRect(player);
    for (auto& p : powerUps) {
        if (!p.active) continue;
        p.timer -= dt;
        if (p.timer <= 0) {
            p.active = false;
            continue;
        }

        Rectangle r = { p.pos.x - p.size/2.0f, p.pos.y - p.size/2.0f, (float)p.size, (float)p.size };
        if (player.active && RectIntersect(pr, r)) ApplyPowerUp(p);
    }

    for (int i = (int)powerUps.size() - 1; i >= 0; i--) {
        if (!powerUps[i].active) powerUps.erase(powerUps.begin() + i);
    }
}

void UpdateBullet(Bullet& b, float dt) {
    if (!b.active) return;
    Vector2 d = DirToVector(b.dir);
    b.pos.x += d.x * b.speed * dt;
    b.pos.y += d.y * b.speed * dt;

    // Cek batas map
    if (b.pos.x < 0 || b.pos.x > SCREEN_WIDTH || b.pos.y < 0 || b.pos.y > SCREEN_HEIGHT) {
        b.active = false;
        return;
    }

    // Cek tabrakan dengan tile
    int tx = (int)(b.pos.x / TILE_SIZE);
    int ty = (int)(b.pos.y / TILE_SIZE);
    if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT) {
        int tile = tileMap[ty][tx];
        if (tile == TILE_BRICK) {
            tileMap[ty][tx] = TILE_EMPTY; // Hancurkan dinding
            b.active = false;
            if (IsAudioDeviceReady()) PlaySound(sndHit);
            return;
        } else if (tile == TILE_STEEL) {
            b.active = false; // Baja tidak hancur
            if (IsAudioDeviceReady()) PlaySound(sndHit);
            return;
        } else if (tile == TILE_BASE) {
            tileMap[ty][tx] = TILE_EMPTY;
            b.active = false;
            gameOver = true;
            playerWon = false;
            if (IsAudioDeviceReady()) PlaySound(sndExplosion);
            return;
        }
    }

    // Cek tabrakan dengan tank
    Rectangle br = { b.pos.x - b.radius, b.pos.y - b.radius, b.radius*2, b.radius*2 };

    // Player hit by enemy bullet
    if (!b.isPlayer && player.active) {
        if (RectIntersect(br, GetTankRect(player))) {
            b.active = false;
            player.hp--;
            if (IsAudioDeviceReady()) PlaySound(sndExplosion);
            if (player.hp <= 0) {
                player.active = false;
                gameOver = true;
                playerWon = false;
            }
            return;
        }
    }

    // Enemy hit by player bullet
    if (b.isPlayer) {
        for (auto& e : enemies) {
            if (e.active && RectIntersect(br, GetTankRect(e))) {
                b.active = false;
                e.hp -= b.damage;
                if (e.hp <= 0) {
                    e.active = false;
                    score += 100;
                    enemiesAlive--;
                    SpawnPowerUp(e.pos);
                    if (IsAudioDeviceReady()) PlaySound(sndExplosion);
                    // Cek menang
                    if (enemiesAlive <= 0 && enemiesToSpawn <= 0) {
                        gameOver = true;
                        playerWon = true;
                    }
                } else {
                    if (IsAudioDeviceReady()) PlaySound(sndHit);
                }
                return;
            }
        }
    }
}

void UpdateEnemyAI(Tank& e, float dt) {
    if (!e.active) return;

    e.changeDirTimer -= dt;
    if (e.changeDirTimer <= 0) {
        e.dir = (Direction)(rand() % 4);
        e.changeDirTimer = 1.0f + (rand() % 200) / 100.0f;
    }

    // Coba gerak
    Vector2 d = DirToVector(e.dir);
    float nx = e.pos.x + d.x * e.speed * dt;
    float ny = e.pos.y + d.y * e.speed * dt;

    if (!TankCollidesWithMap(e, nx, ny) && !TanksCollide(e, player)) {
        bool collideEnemy = false;
        for (auto& other : enemies) {
            if (other.active && &other != &e && TanksCollide(e, other)) {
                // Cek jika posisi baru tidak tabrakan
                Tank temp = e; temp.pos = {nx, ny};
                if (TanksCollide(temp, other)) collideEnemy = true;
            }
        }
        if (!collideEnemy) {
            e.pos.x = nx;
            e.pos.y = ny;
        } else {
            e.dir = (Direction)(rand() % 4); // arah lain
        }
    } else {
        e.dir = (Direction)(rand() % 4); // arah lain jika nabrak dinding/player
    }

    // Tembak acak
    e.shootTimer -= dt;
    if (e.shootTimer <= 0) {
        // 30% kemungkinan tembak
        if ((rand() % 100) < 40) {
            Shoot(e);
        } else {
            e.shootTimer = e.shootCooldown * 0.5f;
        }
    }
}

void UpdateGame(float dt) {
    if (gameOver) {
        if (IsKeyPressed(KEY_ENTER)) {
            // Restart
            InitGame();
        }
        return;
    }

    // Spawn musuh
    if (enemiesAlive < 3 && enemiesToSpawn > 0) {
        spawnTimer -= dt;
        if (spawnTimer <= 0) {
            SpawnEnemy();
            spawnTimer = spawnDelay;
        }
    }

    // Player Input
    if (player.active) {
        player.shootTimer -= dt;
        Direction moveDir = player.dir;
        bool moved = false;
        if (IsKeyDown(KEY_UP))    { moveDir = DIR_UP; moved = true; }
        if (IsKeyDown(KEY_DOWN))  { moveDir = DIR_DOWN; moved = true; }
        if (IsKeyDown(KEY_LEFT))  { moveDir = DIR_LEFT; moved = true; }
        if (IsKeyDown(KEY_RIGHT)) { moveDir = DIR_RIGHT; moved = true; }

        if (moved) {
            player.dir = moveDir;
            Vector2 d = DirToVector(moveDir);
            float nx = player.pos.x + d.x * player.speed * dt;
            float ny = player.pos.y + d.y * player.speed * dt;

            // Clamp di map
            if (nx < player.size/2.0f) nx = player.size/2.0f;
            if (nx > SCREEN_WIDTH - player.size/2.0f) nx = SCREEN_WIDTH - player.size/2.0f;
            if (ny < player.size/2.0f) ny = player.size/2.0f;
            if (ny > SCREEN_HEIGHT - player.size/2.0f) ny = SCREEN_HEIGHT - player.size/2.0f;

            // Cek tabrakan dinding
            if (!TankCollidesWithMap(player, nx, player.pos.y)) player.pos.x = nx;
            if (!TankCollidesWithMap(player, player.pos.x, ny)) player.pos.y = ny;
        }

        // Tembak
        if (IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_Z)) {
            Shoot(player);
        }
    }

    // Update enemies
    for (auto& e : enemies) {
        UpdateEnemyAI(e, dt);
    }

    // Update bullets
    for (auto& b : bullets) {
        UpdateBullet(b, dt);
    }

    UpdatePowerUps(dt);

    // Hapus peluru nonaktif
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        if (!bullets[i].active) bullets.erase(bullets.begin() + i);
    }
}

// ============================================================================
// DRAWING / RENDERING
// ============================================================================
void DrawTileMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Rectangle r = GetTileRect(x, y);
            int t = tileMap[y][x];
            if (t == TILE_BRICK) {
                DrawRectangleRec(r, BEIGE);
                DrawRectangleLinesEx(r, 1, BROWN);
                // Texture brick sederhana (garis)
                DrawLine((int)r.x, (int)(r.y + r.height/2), (int)(r.x + r.width), (int)(r.y + r.height/2), BROWN);
                DrawLine((int)(r.x + r.width/2), (int)r.y, (int)(r.x + r.width/2), (int)(r.y + r.height), BROWN);
            } else if (t == TILE_STEEL) {
                DrawRectangleRec(r, GRAY);
                DrawRectangleLinesEx(r, 2, DARKGRAY);
                DrawRectangle((int)(r.x + 4), (int)(r.y + 4), (int)(r.width - 8), (int)(r.height - 8), LIGHTGRAY);
            } else if (t == TILE_WATER) {
                DrawRectangleRec(r, ColorAlpha(BLUE, 0.6f));
            } else if (t == TILE_BASE) {
                // Gambar "Eagle" / basis
                DrawRectangleRec(r, GOLD);
                DrawRectangleLinesEx(r, 2, ORANGE);
                DrawTriangle(
                    {r.x + r.width/2, r.y + 4},
                    {r.x + 4, r.y + r.height - 4},
                    {r.x + r.width - 4, r.y + r.height - 4},
                    DARKGREEN
                );
            }
        }
    }
}

void DrawTank(const Tank& t) {
    if (!t.active) return;

    int x = (int)t.pos.x;
    int y = (int)t.pos.y;
    int s = t.size;
    int half = s/2;

    Color body = t.color;
    Color bodyDark = ColorBrightness(body, -0.35f);
    Color bodyLight = ColorBrightness(body, 0.25f);
    Color track = Color{35, 35, 35, 255};
    Color barrel = Color{25, 25, 25, 255};

    // Bayangan supaya bentuk kotak tank lebih terbaca.
    DrawRectangle(x - half + 2, y - half + 3, s, s, Color{0, 0, 0, 90});

    // Track/rantai tank. Posisinya tetap mengikuti orientasi tank.
    if (t.dir == DIR_UP || t.dir == DIR_DOWN) {
        DrawRectangle(x - half,     y - half, 6, s, track);
        DrawRectangle(x + half - 6, y - half, 6, s, track);
        for (int yy = y - half + 3; yy < y + half; yy += 7) {
            DrawLine(x - half + 1, yy, x - half + 5, yy, DARKGRAY);
            DrawLine(x + half - 5, yy, x + half - 1, yy, DARKGRAY);
        }
        DrawRectangle(x - half + 6, y - half + 3, s - 12, s - 6, body);
    } else {
        DrawRectangle(x - half, y - half,     s, 6, track);
        DrawRectangle(x - half, y + half - 6, s, 6, track);
        for (int xx = x - half + 3; xx < x + half; xx += 7) {
            DrawLine(xx, y - half + 1, xx, y - half + 5, DARKGRAY);
            DrawLine(xx, y + half - 5, xx, y + half - 1, DARKGRAY);
        }
        DrawRectangle(x - half + 3, y - half + 6, s - 6, s - 12, body);
    }

    // Badan utama kotak.
    DrawRectangleLines(x - half, y - half, s, s, bodyDark);
    DrawRectangle(x - 7, y - 7, 14, 14, bodyLight);       // turret/kubah tengah berbentuk kotak
    DrawRectangleLines(x - 7, y - 7, 14, 14, bodyDark);

    // Laras senjata: kotak panjang yang menonjol jelas ke arah tank.
    int barrelW = 6;
    int barrelLen = half + 12;
    switch (t.dir) {
        case DIR_UP:
            DrawRectangle(x - barrelW/2, y - half - 10, barrelW, barrelLen, barrel);
            DrawRectangle(x - barrelW/2 - 1, y - half - 12, barrelW + 2, 4, barrel);
            break;
        case DIR_DOWN:
            DrawRectangle(x - barrelW/2, y - 2, barrelW, barrelLen, barrel);
            DrawRectangle(x - barrelW/2 - 1, y + half + 8, barrelW + 2, 4, barrel);
            break;
        case DIR_LEFT:
            DrawRectangle(x - half - 10, y - barrelW/2, barrelLen, barrelW, barrel);
            DrawRectangle(x - half - 12, y - barrelW/2 - 1, 4, barrelW + 2, barrel);
            break;
        case DIR_RIGHT:
            DrawRectangle(x - 2, y - barrelW/2, barrelLen, barrelW, barrel);
            DrawRectangle(x + half + 8, y - barrelW/2 - 1, 4, barrelW + 2, barrel);
            break;
    }

    // Detail kecil seperti lampu depan sesuai arah.
    Vector2 d = DirToVector(t.dir);
    Vector2 p = { -d.y, d.x };
    DrawCircle((int)(x + d.x*8 + p.x*5), (int)(y + d.y*8 + p.y*5), 2, YELLOW);
    DrawCircle((int)(x + d.x*8 - p.x*5), (int)(y + d.y*8 - p.y*5), 2, YELLOW);

    // Indikator HP dan level untuk player.
    if (t.isPlayer) {
        DrawText(TextFormat("HP:%i", t.hp), x - 20, y - half - 16, 10, WHITE);
        DrawText(TextFormat("Lv%i", t.level), x - 12, y + half + 4, 10, YELLOW);
    }
}

void DrawBullet(const Bullet& b) {
    if (!b.active) return;
    Color c = b.isPlayer ? YELLOW : ORANGE;
    DrawCircle((int)b.pos.x, (int)b.pos.y, b.radius, c);
    DrawCircle((int)b.pos.x, (int)b.pos.y, b.radius - 1, WHITE);
}


void DrawPowerUp(const PowerUp& p) {
    if (!p.active) return;
    int x = (int)p.pos.x;
    int y = (int)p.pos.y;
    int h = p.size / 2;

    Color c = WHITE;
    const char* label = "?";
    switch (p.type) {
        case POWER_GUN:   c = SKYBLUE; label = "G"; break; // gun upgrade
        case POWER_HP:    c = LIME;    label = "+"; break; // heal
        case POWER_SPEED: c = GOLD;    label = "S"; break; // speed
        case POWER_ARMOR: c = PURPLE;  label = "A"; break; // armor/max HP
    }

    // Efek berkedip saat hampir hilang.
    if (p.timer < 3.0f && ((int)(p.timer * 8) % 2 == 0)) return;

    DrawRectangle(x - h, y - h, p.size, p.size, c);
    DrawRectangleLines(x - h, y - h, p.size, p.size, WHITE);
    int tw = MeasureText(label, 18);
    DrawText(label, x - tw/2, y - 9, 18, BLACK);
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(Color{30, 40, 20, 255}); // Dark army green background

    DrawTileMap();

    // Draw power-up items
    for (auto& p : powerUps) DrawPowerUp(p);

    // Draw bullets
    for (auto& b : bullets) DrawBullet(b);

    // Draw enemies
    for (auto& e : enemies) DrawTank(e);

    // Draw player
    DrawTank(player);

    // UI
    DrawRectangle(0, 0, SCREEN_WIDTH, 28, Color{20,20,20,200});
    DrawText(TextFormat("SCORE:%i  ENEMY:%i/%i  HP:%i/%i  LV:%i  SHOT:%i  MAXB:%i", score, enemiesAlive, enemiesToSpawn + enemiesAlive, player.hp, player.maxHp, player.level, player.shotCount, player.maxBullets), 10, 6, 18, WHITE);
    DrawText("ARROWS: Move | SPACE/Z: Shoot | Items: G gun, + heal, S speed, A armor", SCREEN_WIDTH - 560, 8, 16, ColorAlpha(WHITE, 0.75f));

    if (gameOver) {
        const char* msg = playerWon ? "YOU WIN!" : "GAME OVER";
        Color msgColor = playerWon ? GREEN : RED;
        int tw = MeasureText(msg, 60);
        DrawRectangle(0, SCREEN_HEIGHT/2 - 50, SCREEN_WIDTH, 110, Color{0,0,0,180});
        DrawText(msg, SCREEN_WIDTH/2 - tw/2, SCREEN_HEIGHT/2 - 40, 60, msgColor);
        int tw2 = MeasureText("Press ENTER to Restart", 24);
        DrawText("Press ENTER to Restart", SCREEN_WIDTH/2 - tw2/2, SCREEN_HEIGHT/2 + 30, 24, WHITE);
    }

    EndDrawing();
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tank Battle - Raylib C++");
    SetTargetFPS(FPS);
    InitAudioDevice();

    // Generate suara secara prosedural (tidak perlu file eksternal)
    // Shoot sound: noise pendek
    Wave wShoot = GenToneWave(WAVE_SAWTOOTH, 880.0f, 0.08f);
    sndShoot = LoadSoundFromWave(wShoot);
    UnloadWave(wShoot);
    SetSoundVolume(sndShoot, 0.4f);

    // Hit sound: square wave pendek
    Wave wHit = GenToneWave(WAVE_SQUARE, 440.0f, 0.06f);
    sndHit = LoadSoundFromWave(wHit);
    UnloadWave(wHit);
    SetSoundVolume(sndHit, 0.3f);

    // Explosion: sawtooth rendah
    Wave wExp = GenToneWave(WAVE_SAWTOOTH, 110.0f, 0.25f);
    sndExplosion = LoadSoundFromWave(wExp);
    UnloadWave(wExp);
    SetSoundVolume(sndExplosion, 0.5f);

    InitGame();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateGame(dt);
        DrawGame();
    }

    UnloadSound(sndShoot);
    UnloadSound(sndHit);
    UnloadSound(sndExplosion);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

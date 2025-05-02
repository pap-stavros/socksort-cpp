#include <raylib.h>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <cmath>

// tickticktick
Wave GenerateComparisonSound() {
    int sampleRate = 44100;
    float duration = 0.05f;
    int sampleCount = sampleRate * duration;
    
    Wave wave = {0};
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(sampleCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    
    // sine quick tjhimng
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float amplitude = 32000.0f * exp(-t * 50);
        float frequency = 440.0f; 
        
        samples[i] = (short)(amplitude * sin(2 * PI * frequency * t));
    }
    
    return wave;
}

// swooooosh
Wave GenerateSwapSound() {
    int sampleRate = 44100;
    float duration = 0.1f;
    int sampleCount = sampleRate * duration;
    
    Wave wave = {0};
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(sampleCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    
    // swoosh again but different
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float amplitude = 24000.0f * (1.0f - t / duration);
        float frequency = 300.0f + 700.0f * t / duration;
        
        samples[i] = (short)(amplitude * sin(2 * PI * frequency * t));
    }
    
    return wave;
}

// goes ping ping ping
Wave GenerateSortedSound() {
    int sampleRate = 44100;
    float duration = 0.2f;
    int sampleCount = sampleRate * duration;
    
    Wave wave = {0};
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(sampleCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    
    // yeat bell
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float amplitude = 28000.0f * exp(-t * 15);
        
        // harmonics i think?
        float base = 880.0f;
        float signal = 0.7f * sin(2 * PI * base * t) +
                      0.2f * sin(2 * PI * base * 2 * t) +
                      0.1f * sin(2 * PI * base * 3 * t);
        
        samples[i] = (short)(amplitude * signal);
    }
    
    return wave;
}

// settings
const int screen_width = 1280;
const int screen_height = 720;
const int laundry_count = 120;

// tracking variables
std::vector<bool> sorted_items;
long long comparisons = 0;
long long swaps = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

// audio objects
Sound comparisonSound;
Sound swapSound;
Sound sortedSound;
int max_height = 0;  // pitch scaling thing

// draws stuff
void draw_laundry(
    const std::vector<int>& laundry,
    int item_a, int item_b,       // highlighted items (red)
    int current,                  // current position (green)
    int lo, int hi,               // partition bounds
    int left, int right,          // partition pointers (gold/orange)
    int depth,                    // recursion depth
    const char* msg               // status message
) {
    BeginDrawing();
    ClearBackground(BLACK);

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    int bar_w = w / laundry_count;
    int extra = w % laundry_count;

    // highlight current group partition thing
    if (lo >= 0 && hi < laundry_count) {
        int part_x = lo * bar_w + std::min(lo, extra);
        int part_w = 0;
        
        for (int i = lo; i <= hi; i++) {
            part_w += bar_w + (i < extra ? 1 : 0);
        }
        
        DrawRectangle(part_x, 0, part_w, h, Fade(GRAY, 0.3f));
    }

    // draw each laundry as bar
    for (int i = 0; i < laundry_count; i++) {
        int bar_h = laundry[i];
        Color color = RAYWHITE;

        // color based bars
        if (i == item_a || i == item_b)
            color = RED;
        else if (i == current)
            color = GREEN;
        else if (i == left)
            color = GOLD;
        else if (i == right)
            color = ORANGE;
        else if (i == lo || i == hi)
            color = PURPLE;
        else if (sorted_items[i])
            color = BLUE;

        int x_pos = i * bar_w + std::min(i, extra);
        int this_w = bar_w + (i < extra ? 1 : 0);

        DrawRectangle(x_pos, h - bar_h, this_w, bar_h, color);
    }

    // show stats
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

    std::string stats = "comparisons: " + std::to_string(comparisons) +
                      " swaps: " + std::to_string(swaps) +
                      " time: " + std::to_string(elapsed) + "ms" +
                      " depth: " + std::to_string(depth);

    DrawText(stats.c_str(), 10, 10, 14, Color{200, 200, 200, 255});
    DrawText(msg, 10, 30, 18, Color{150, 150, 150, 180});
    
    EndDrawing();
}

void PlaySoundWithPitch(Sound sound, int value) {
    //pitch calc slang for calculator btw
    float pitchFactor = 0.5f + ((float)value / max_height) * 1.5f;
    SetSoundPitch(sound, pitchFactor);
    PlaySound(sound);
}

// the sock-finding algoalgoalgo
void sock_sort(std::vector<int>& laundry) {
    // Reset tracking variables
    sorted_items.assign(laundry_count, false);
    comparisons = swaps = 0;
    start_time = std::chrono::high_resolution_clock::now();

    // Stack for sorting sections - start with whole pile
    std::vector<std::pair<int, int>> stack;
    stack.push_back({0, laundry_count - 1});
    
    std::mt19937 rng(std::random_device{}());

    // Keep sorting until no sections left
    while (!stack.empty()) {
        // Get section to sort
        int lo = stack.back().first;
        int hi = stack.back().second;
        stack.pop_back();
        
        // Skip if section is too small
        if (lo >= hi) continue;

        // pick random stuff as sock a and sock b
        std::uniform_int_distribution<> dist(lo, hi);
        int item_a = dist(rng);
        int item_b = dist(rng);

        // move socks to ends
        std::swap(laundry[lo], laundry[item_a]); 
        swaps++;
        PlaySoundWithPitch(swapSound, laundry[lo]);
        
        std::swap(laundry[hi], laundry[item_b]); 
        swaps++;
        PlaySoundWithPitch(swapSound, laundry[hi]);

        // ensure low <= high item
        comparisons++;
        PlaySoundWithPitch(comparisonSound, (laundry[lo] + laundry[hi]) / 2);
        
        if (laundry[lo] > laundry[hi]) {
            std::swap(laundry[lo], laundry[hi]);
            swaps++;
            PlaySoundWithPitch(swapSound, laundry[lo]);
        }

        int L = laundry[lo];  // left sock va;
        int R = laundry[hi];  // right sock val
        
        int left = lo + 1;    // left boundary
        int right = hi - 1;   // right boundary
        int pos = lo + 1;     // current position

        // triple partition time
        while (pos <= right) {
            comparisons++;
            PlaySoundWithPitch(comparisonSound, laundry[pos]);

            if (laundry[pos] < L) {
                // go to left group
                std::swap(laundry[left++], laundry[pos++]);
                swaps++;
                PlaySoundWithPitch(swapSound, laundry[left-1]);
            } 
            else if (laundry[pos] > R) {
                // go to right group
                std::swap(laundry[pos], laundry[right--]);
                swaps++;
                PlaySoundWithPitch(swapSound, laundry[right+1]);
            } 
            else {
                // otherwise stay middle
                pos++;
            }

            draw_laundry(laundry, item_a, item_b, pos, lo, hi, left, right, stack.size(), "sorting laundry for socks");
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        left--;
        right++;

        // final sock position or sm
        std::swap(laundry[lo], laundry[left]); 
        swaps++;
        PlaySoundWithPitch(swapSound, laundry[left]);
        
        std::swap(laundry[hi], laundry[right]); 
        swaps++;
        PlaySoundWithPitch(swapSound, laundry[right]);

        // socks = sorted
        sorted_items[left] = true;
        sorted_items[right] = true;
        
        // sorted sound for both socks
        PlaySoundWithPitch(sortedSound, laundry[left]);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        PlaySoundWithPitch(sortedSound, laundry[right]);

        draw_laundry(laundry, item_a, item_b, -1, lo, hi, left, right, stack.size(), "laundry pile sorted");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // emaining partitions
        stack.push_back({right + 1, hi});        // right partition
        stack.push_back({left + 1, right - 1});  // middle partition
        stack.push_back({lo, left - 1});         // left partition
    }

    // all done!
    for (int i = 0; i < laundry_count; i++) {
        sorted_items[i] = true;
    }

    draw_laundry(laundry, -1, -1, -1, 0, laundry_count - 1, -1, -1, 0, "found all the socks!");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // final sweep
    for (int i = 0; i < laundry_count; i++) {
        draw_laundry(laundry, -1, -1, i, 0, laundry_count - 1, -1, -1, 0, "matching socks");
        
        if (i % 5 == 0) { // play every fifth cause weird
            PlaySoundWithPitch(sortedSound, laundry[i]);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    draw_laundry(laundry, -1, -1, -1, 0, laundry_count - 1, -1, -1, 0, "all socks paired up!");
}

int main() {
    InitWindow(screen_width, screen_height, "sock sort - finding socks in laundry");
    InitAudioDevice();
    SetTargetFPS(60);

    // sound gen
    Wave comparisonWave = GenerateComparisonSound();
    comparisonSound = LoadSoundFromWave(comparisonWave);
    
    Wave swapWave = GenerateSwapSound();
    swapSound = LoadSoundFromWave(swapWave);
    
    Wave sortedWave = GenerateSortedSound();
    sortedSound = LoadSoundFromWave(sortedWave);

    // crng laundry pile aka array
    std::vector<int> laundry(laundry_count);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> height_range(10, screen_height);

    for (int& item : laundry) {
        item = height_range(rng);
        max_height = std::max(max_height, item); // pitch scale w height
    }

    sock_sort(laundry);

    // Keep window open after sort is done
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        int bar_w = screen_width / laundry_count;
        int extra = screen_width % laundry_count;

        for (int i = 0; i < laundry_count; i++) {
            int this_w = bar_w + (i < extra ? 1 : 0);
            int x_pos = i * bar_w + std::min(i, extra);
            
            DrawRectangle(x_pos, screen_height - laundry[i], this_w, laundry[i], BLUE);
        }

        EndDrawing();
    }

    // Clean up
    UnloadWave(comparisonWave);
    UnloadSound(comparisonSound);
    
    UnloadWave(swapWave);
    UnloadSound(swapSound);
    
    UnloadWave(sortedWave);
    UnloadSound(sortedSound);
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
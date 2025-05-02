# Sock Sort Algorithm

I came up with this algorithm while looking for socks in my laundry basket. It is based on my "sock finding method" where I basically pick two random socks from the basket and then try looking for a match. The rest of the logic came from some metaphors in my head that I don't even remember now. I haven't touched sorting algorithms in years but this was really fun. I'm sure it's not even remotely optimal or even original but it works and it's mine :)

## How It Works

The algorithm works by randomly selecting two "socks" from the laundry basket (array), then sorting items into three groups:

1. Socks smaller than the lower sock go to the left.
2. Socks larger than the upper sock go to the right.
3. The socks in between stay in the middle.

It keeps doing this recursively until everything is sorted.

## Building from Source

### Prerequisites

- A C++ compiler, i guess.
- [Raylib](https://www.raylib.com/) - Optional
(I've put the sorting algorithm in a separate file)

### Compilation:
```bash
#Linux/MacOs
g++ -o build socksort_visual.cpp -lraylib -lm -lpthread -ldl -lX11

#Windows
g++ -o build.exe socksort_visual.cpp -lraylib -lopengl32 -lgdi32 -lwinmm

// Maps a value from one range to another
// taken from Processing: https://stackoverflow.com/a/17135426
float map(float n, float min1, float max1, float min2, float max2) {
    return min2 + (max2 - min2) * ((n - min1) / (max1 - min1));
}

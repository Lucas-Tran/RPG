#include <Animation.h>


Animation::Animation(std::string path, float frameDelay):
frameDelay(frameDelay) {
    Image gifImage = LoadImageAnim(path.c_str(), &totalAnimationFrames);
     if (gifImage.data == nullptr) {
         throw std::runtime_error("Failed to load GIF image:" + path);
     }

    frames = new Texture2D[totalAnimationFrames];

    constexpr int NUM_CHANNELS = 4;
    int frameSizeInBytes = gifImage.width * gifImage.height * NUM_CHANNELS;

    // 3. Extract each frame individual slice and upload it to VRAM permanently
    for (int i = 0; i < totalAnimationFrames; i++) {
        int memoryOffset = frameSizeInBytes * i;

        // Isolate the single frame data slice using your pointer arithmetic
        Image frameSlice = {
            .data = static_cast<unsigned char*>(gifImage.data) + memoryOffset,
            .width = gifImage.width,
            .height = gifImage.height,
            .mipmaps = 1,
            .format = gifImage.format
        };

        Image singleFrame = ImageCopy(frameSlice);

        constexpr int SOURCE_IMAGE_SIZE = 16;

        int scaleFactor = TILE_SIZE / SOURCE_IMAGE_SIZE;

        ImageResizeNN(&singleFrame, singleFrame.width * scaleFactor, singleFrame.height * scaleFactor);

        frames[i] = LoadTextureFromImage(singleFrame);

        UnloadImage(singleFrame);
    }
    UnloadImage(gifImage);
}

Animation::~Animation() {
    for (int i = 0; i < totalAnimationFrames; i++) {
        UnloadTexture(frames[i]);
    }
    delete[] frames;
}

Texture2D Animation::GetFirstFrame() {
    return frames[0];
}

void Animation::Draw(Vector2 position, float animTime) {
    Draw(position, animTime, WHITE);
}

void Animation::Draw(Vector2 position, float animTime, Color tint) {
    int currentFrame = static_cast<int>(animTime / frameDelay) % totalAnimationFrames;
    DrawTextureV(frames[currentFrame], position, tint);
}

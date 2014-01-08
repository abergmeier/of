#include "EMVideoPlayer.hpp"

using namespace of::video::emscripten;

bool
VideoPlayer::loadMovie( string name ) {
	return false;
}

void
VideoPlayer::close() {
}

void
VideoPlayer::update() {
}

void
VideoPlayer::play() {
}

void
VideoPlayer::stop() {
}

bool
VideoPlayer::isFrameNew() {
	return false;
}

unsigned char*
VideoPlayer::getPixels() {
	return nullptr;
}

ofTexture*
VideoPlayer::getTexture() {
	return nullptr;
}

float
VideoPlayer::getWidth() {
	return 0;
}

float
VideoPlayer::getHeight() {
	return 0;
}

bool
VideoPlayer::isPaused() {
	return false;
}

bool
VideoPlayer::isLoaded() {
	return false;
}

bool
VideoPlayer::isPlaying() {
	return false;
}

bool
VideoPlayer::setPixelFormat( ofPixelFormat pixelFormat ) {
	return false;
}

ofPixelFormat
VideoPlayer::getPixelFormat() {
	return ofPixelFormat::OF_PIXELS_UNKNOWN;
}

ofPixels_<unsigned char>&
VideoPlayer::getPixelsRef() {
	return ofPixels_<unsigned char>{};
}


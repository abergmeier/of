#pragma once

namespace of {
	namespace video {
		namespace emscripten {
			class VideoPlayer : public ofBaseVideoPlayer {
			public:
	
				bool loadMovie( string name ) override;
				void close() override;
				void update() override;
	
				void play() override;
				void stop() override;
	
				bool isFrameNew() override;
				unsigned char* getPixels() override;
				ofTexture*     getTexture() override;
	
				float          getWidth() override;
				float          getHeight() override;
	
				bool           isPaused() override;
				bool           isLoaded() override;
				bool           isPlaying() override;
	
				bool           setPixelFormat( ofPixelFormat pixelFormat ) override;
				ofPixelFormat  getPixelFormat() override;
				ofPixels_<unsigned char>&  getPixelsRef() override;
			};
		} // namespace emscripten
	} // namespace video
} // namespace of


/*==============================================================================

 Copyright (c) 2013, 2014 Andreas Bergmeier

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 modified by Philip Whitfield (http://www.undef.ch)

 ==============================================================================*/

#pragma once

#include "BaseEGLWindow.hpp"
#include <array>

namespace of {
	using std::array;
	using std::unique_ptr;
	namespace emscripten {
		class EGLPage : public BaseEGLWindow {
		public:
			typedef BaseEGLWindow base_type;
			struct Settings : public base_type::Settings {
				Settings();
				EGLint eglCanvasOpacity; // 0-255 window alpha value
			};
			EGLPage() noexcept;
			EGLPage( Settings settings );
			EGLPage( EGLPage&& ) = default;
			~EGLPage() noexcept;
			EGLPage& operator=( EGLPage&& ) = default;
			void                 setGLESVersion( int glesVersion );
			void                 setupOpenGL( int w, int h, int screenMode ) override;
			void                 runAppViaInfiniteLoop( ofBaseApp * appPtr );

			// Orientation handling
			ofOrientation        getOrientation() override;
			bool                 doesHWOrientation() override;


		protected:

			void display();

			EGLNativeWindowType  getNativeWindow() override;
			EGLNativeDisplayType getNativeDisplay() override;

			void checkEvents();
		private:
			void main();
			array<int, 2> nonFullscreenSize;
			float _canvasOpacity;

			void setupPeripherals();
		};
	} // namespace emscripten
} // namespace of


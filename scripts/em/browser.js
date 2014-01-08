mergeInto(
	LibraryManager.library, {
		of: { orientation: { last: { alpha: 0
		} } },
		of_device_z_rotation_deg: function() {
			return of.orientation.last.alpha;
		},
		emscripten_set_canvas_opacity: function( opacity ) {
			Module.canvas.style.opacity = opacity;
		},
		start_infinite_loop__deps: ['emscripten_cancel_main_loop', 'emscripten_set_main_loop'],
		start_infinite_loop: function( funcptr ) {

			function handleOrientation(event) {
				of.orientation.last.alpha = event.alpha;
			}

			window.addEventListener( "deviceorientation", handleOrientation, true );

			function start_main_loop( first_call ) {
				var TIMEOUT = { visible: -1, non_visible: 500 };
				_emscripten_set_main_loop( funcptr, (document.hidden) ? TIMEOUT.visible : TIMEOUT.non_visible, first_call );
			}

			// Disable rapid rendering, should the window not be displayed
			function visibilityChanged() {
				_emscripten_cancel_main_loop();
				start_main_loop( false );
			}

			document.addEventListener( "visibilitychange", visibilityChanged );
			start_main_loop( true );
		},
	}
);


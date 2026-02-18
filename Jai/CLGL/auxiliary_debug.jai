#import "Debug";
#import "Basic";
#load "gl.jai";

debug_log_params : struct {
    logger: Logger;
};

DebugCallback :: (source: old.GLenum, type: old.GLenum, id: old.GLuint, severity: old.GLenum, length: u32, message: *u8, userParam: *void) #c_call {

    push_context {
        assert(userParam == *debug_log_params);

        if type==old.GL_DEBUG_TYPE_ERROR then print("***ERROR*** ");
        print("GL_CALLBACK: %\n", to_string(message));
        //breakpoint();
    }
}

enable_debug_output :: () {
    //attach_to_debugger();
    debug_log_params.logger = context.logger;
    old.glEnable(old.GL_DEBUG_OUTPUT); //Enable Debug
    old.glEnable(old.GL_DEBUG_OUTPUT_SYNCHRONOUS); //Make Debug Synchronous
    old.glDebugMessageCallback(DebugCallback, *debug_log_params); //Specify Callback Function
    old.glDebugMessageControl(old.GL_DONT_CARE, old.GL_DONT_CARE, old.GL_DEBUG_SEVERITY_NOTIFICATION, xx 0, xx 0, old.GL_FALSE); //Disable Non-Error Notifications
}
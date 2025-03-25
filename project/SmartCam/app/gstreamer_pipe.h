#ifndef __GSTREAM_PIPE_H__
#define __GSTREAM_PIPE_H__

int gstreamer_start_pipe();

int gstreamer_stop_pipe();

int gstreamer_create_pipeline(unsigned width, unsigned height);

void gstreamer_set_pipeline_resolution(int width, int height);

void gstream_loop();
#endif

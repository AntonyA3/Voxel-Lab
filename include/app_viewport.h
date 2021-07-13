typedef struct AppViewport{
    float x, y, width, height;
    float ratio;
}AppViewport;

void init_app_viewport(AppViewport* appViewport, int x, int y, int width, int height);
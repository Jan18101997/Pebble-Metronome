#include <pebble.h>
#include "main_window.h"

#define MAX_BPM 300
#define MIN_BPM 0

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static TextLayer *s_textlayer_current_bpm;
static TextLayer *s_textlayer_bpm;
static int currentBPM = 80;
static AppTimer *timer;
static int8_t mode = 0;
static bool colorIsInverted = false;
static uint32_t lastTick = 0;

static void timer_callback();
static void startTimer();

uint32_t now() {
  time_t s = 0;
  uint16_t ms = 0;
  time_ms(&s, &ms);
  return s * 1000l + ms;
}

static const uint32_t vibeSegments[] = { 50 };
VibePattern vibePattern = {
  .durations = vibeSegments,
  .num_segments = ARRAY_LENGTH(vibeSegments),
};

static void display_current_bpm()
{
  static char buffer[8];
  
  snprintf(buffer, sizeof(buffer), "%d", currentBPM);
  text_layer_set_text(s_textlayer_current_bpm, buffer);
}

static void updateColor()
{
  if(colorIsInverted)
  {
    window_set_background_color(s_window, GColorWhite);
    text_layer_set_text_color(s_textlayer_current_bpm, GColorBlack);
    text_layer_set_text_color(s_textlayer_bpm, GColorBlack);
  }
  else
  {
    window_set_background_color(s_window, GColorBlack);
    text_layer_set_text_color(s_textlayer_current_bpm, GColorWhite);
    text_layer_set_text_color(s_textlayer_bpm, GColorWhite);
  }
}

static void timer_callback() 
{
  colorIsInverted = !colorIsInverted;
  
  updateColor();

  startTimer();
  
  vibes_enqueue_custom_pattern(vibePattern);
}

static void startTimer()
{
  app_timer_cancel(timer);
  
  if(mode == 1)
  {
    int millis_gap = 60000 / currentBPM;
    
    millis_gap = lastTick + millis_gap - now();
    
    if(millis_gap < 0)
      millis_gap = 0;
    
    lastTick = now();
    
    timer = app_timer_register(millis_gap, timer_callback, NULL);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  if(currentBPM < MAX_BPM)
  {
    currentBPM++;
    display_current_bpm();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  if(mode == 0)
  {
    mode = 1;
    lastTick = now();
    startTimer();
  }
  else
  {
    if(colorIsInverted)
    {
      colorIsInverted = false;
      updateColor();
    }
    
    mode = 0;
    app_timer_cancel(timer);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  if(currentBPM > MIN_BPM)
  {
    currentBPM--;
    display_current_bpm();
  }
}

static void click_config_provider(void *context) 
{
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  
  // s_textlayer_current_bpm
  s_textlayer_current_bpm = text_layer_create(GRect(0, 40, 144, 42));
  text_layer_set_background_color(s_textlayer_current_bpm, GColorClear);
  text_layer_set_text_color(s_textlayer_current_bpm, GColorWhite);
  text_layer_set_text(s_textlayer_current_bpm, "80");
  text_layer_set_text_alignment(s_textlayer_current_bpm, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_current_bpm, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_current_bpm);
  
  // s_textlayer_bpm
  s_textlayer_bpm = text_layer_create(GRect(0, 81, 144, 25));
  text_layer_set_background_color(s_textlayer_bpm, GColorClear);
  text_layer_set_text_color(s_textlayer_bpm, GColorWhite);
  text_layer_set_text(s_textlayer_bpm, "bpm");
  text_layer_set_text_alignment(s_textlayer_bpm, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_bpm, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_bpm);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_current_bpm);
  text_layer_destroy(s_textlayer_bpm);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void create_main_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_set_click_config_provider(s_window, click_config_provider);
  window_stack_push(s_window, true);
}

void delete_main_window(void) {
  window_stack_remove(s_window, true);  
  destroy_ui();
}

int main(void) 
{
  create_main_window();
  app_event_loop();
  delete_main_window();
}

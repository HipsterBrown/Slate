#include <pebble.h>

#define NEW_USER_PKEY 1
#define NEW_USER_DEFAULT true

Window *my_window;
Window *help_window;
TextLayer *s_output_layer;
TextLayer *s_help_layer;
StatusBarLayer *s_status_bar;
ActionBarLayer *s_action_bar;
ActionBarLayer *s_help_action_bar;
GBitmap *s_mic_bitmap;
GBitmap *s_check_bitmap;

DictationSession *s_dictation_session;
char s_last_text[512];

bool s_new_user = NEW_USER_DEFAULT;
bool s_continue_reminder = false;

bool send_to_phone(char *transcription) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  Tuplet tuple = TupletCString(0x0, transcription);
  dict_write_tuplet(iter, &tuple);
  dict_write_end(iter);
  
  app_message_outbox_send();
  return true;
}

void timer_callback(void *context) {
  text_layer_set_text(s_output_layer, PBL_IF_ROUND_ELSE("Set A Reminder ->", "Set A Reminder"));
}

void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *message_tuple = dict_find(iter, 0x0);
  
  strcpy(s_last_text, message_tuple->value->cstring);
  text_layer_set_text(s_output_layer, s_last_text);
  
  app_timer_register(10000, timer_callback, NULL);
}

void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped");
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}

void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  if (status == DictationSessionStatusSuccess) {
    send_to_phone(transcription);
  } else {
    text_layer_set_text(s_output_layer, PBL_IF_ROUND_ELSE("Set A Reminder ->", "Set A Reminder"));
  }
}

void help_window_pop() {
  s_new_user ? APP_LOG(APP_LOG_LEVEL_DEBUG, "New user!") : APP_LOG(APP_LOG_LEVEL_DEBUG, "Onboarded user!");
  window_stack_pop(true);
  window_destroy(help_window);
}

void help_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_new_user = false;
  help_window_pop();
}

void help_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, help_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, help_click_handler);
}

void help_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int16_t width = bounds.size.w - ACTION_BAR_WIDTH;
  
  s_check_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CHECK);
  
  s_help_action_bar = action_bar_layer_create();
  action_bar_layer_set_icon_animated(s_help_action_bar, BUTTON_ID_SELECT, s_check_bitmap, true);
  action_bar_layer_set_background_color(s_help_action_bar, GColorElectricUltramarine);
  action_bar_layer_set_click_config_provider(s_help_action_bar, help_click_config_provider);
  
  s_help_layer = text_layer_create(GRect(bounds.origin.x, bounds.origin.y + PBL_IF_ROUND_ELSE(60, 50), width - 5, bounds.size.h));
  text_layer_set_text(s_help_layer, PBL_IF_ROUND_ELSE("Example Reminder:\n\n\"Remind me to pick up milk at 5pm\" ", "Example Reminder:\n\n\"Remind me to pick up milk at 5pm\" "));
  text_layer_set_font(s_help_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_help_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_help_layer));
  action_bar_layer_add_to_window(s_help_action_bar, window);
}

void help_window_unload(Window *window) {
  text_layer_destroy(s_help_layer);
  dictation_session_start(s_dictation_session);
}

void help_window_push() {
  if(!help_window) {
    help_window = window_create();
    window_set_click_config_provider(help_window, help_click_config_provider);
    window_set_window_handlers(help_window, (WindowHandlers) {
      .load = help_window_load,
      .unload = help_window_unload
    });
    window_stack_push(help_window, true);
  }
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_continue_reminder = true;
  s_new_user ? help_window_push() : dictation_session_start(s_dictation_session);
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  help_window_push();
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int16_t width = bounds.size.w - ACTION_BAR_WIDTH;
  GRect frame = GRect(0, 0, width, STATUS_BAR_LAYER_HEIGHT);
  
  s_mic_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MIC);
  
  s_action_bar = action_bar_layer_create();
  action_bar_layer_set_icon_animated(s_action_bar, BUTTON_ID_SELECT, s_mic_bitmap, true);
  action_bar_layer_set_background_color(s_action_bar, GColorElectricUltramarine);
  action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
  
  s_status_bar = status_bar_layer_create();
  status_bar_layer_set_colors(s_status_bar, GColorWhite, GColorBlack);
  
  #if defined(PBL_RECT)
    layer_set_frame(status_bar_layer_get_layer(s_status_bar), frame);
  #endif
  
  s_output_layer = text_layer_create(GRect(bounds.origin.x, bounds.origin.y + PBL_IF_ROUND_ELSE(72, 66), width - 2, bounds.size.h));
  text_layer_set_text(s_output_layer, PBL_IF_ROUND_ELSE("Set A Reminder ->", "Set A Reminder"));
  text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(s_output_layer, GTextAlignmentCenter);
  
  // add all the layers
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
  action_bar_layer_add_to_window(s_action_bar, window);
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
}

void window_unload(Window *window) {
  text_layer_destroy(s_output_layer);
}


void handle_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  
  app_message_open(512, 512);
  s_new_user = persist_exists(NEW_USER_PKEY) ? persist_read_bool(NEW_USER_PKEY) : NEW_USER_DEFAULT;
  
  my_window = window_create();
  window_set_click_config_provider(my_window, click_config_provider);
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(my_window, true);
  
  s_dictation_session = dictation_session_create(sizeof(s_last_text), dictation_session_callback, NULL);
}

void handle_deinit(void) {
  persist_write_bool(NEW_USER_PKEY, s_new_user);
  dictation_session_destroy(s_dictation_session);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

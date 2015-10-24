#include <pebble.h>

Window *my_window;
TextLayer *s_output_layer;
StatusBarLayer *s_status_bar;
ActionBarLayer *s_action_bar;
GBitmap *s_mic_bitmap;

DictationSession *s_dictation_session;
char s_last_text[512];

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
    snprintf(s_last_text, sizeof(s_last_text), "Transcription:\n\n%s", transcription);
    text_layer_set_text(s_output_layer, s_last_text);
    
    send_to_phone(transcription);
  } else {
    text_layer_set_text(s_output_layer, PBL_IF_ROUND_ELSE("Set A Reminder ->", "Set A Reminder"));
  }
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  dictation_session_start(s_dictation_session);
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
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
  dictation_session_destroy(s_dictation_session);
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

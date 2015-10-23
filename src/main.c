#include <pebble.h>

Window *my_window;
TextLayer *s_output_layer;

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

void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *message_tuple = dict_find(iter, 0x0);
  
  strcpy(s_last_text, message_tuple->value->cstring);
  text_layer_set_text(s_output_layer, s_last_text);
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
    char s_failed_buff[128];
    snprintf(s_failed_buff, sizeof(s_failed_buff), "Transcription failed:\n\n%d", (int)status);
    text_layer_set_text(s_output_layer, s_failed_buff);
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
  
  s_output_layer = text_layer_create(GRect(bounds.origin.x, (bounds.size.h - 24) / 2, bounds.size.w, bounds.size.h));
  text_layer_set_text(s_output_layer, "Press Select to get input!");
  text_layer_set_text_alignment(s_output_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
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

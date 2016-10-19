#include <pebble.h>

//#define DEBUG 

static const char* const QLOCKTWO[] = {
  "ITLISASTIME",
  "ACQUARTERDC",
  "TWENTYFIVEX",
  "HALFBTENFTO",
  "PASTERUNINE",
  "ONESIXTHREE",
  "FOURFIVETWO",
  "EIGHTELEVEN",
  "SEVENTWELVE",
  "TENSEOCLOCK"
};

static char qlocktwoChar[10][11][2];

static Window *window;
static TextLayer *qlocktwo_layer[10][11];
static GFont eq_font;
static GFont lt_font;
static GColor eq_bg;
static GColor eq_text;
static GColor eq_active;
static int prevMin = -1;
static unsigned short matrix[10];

//static const HealthMetric metric = HealthMetricStepCount;

inline void resetMatrix(unsigned short* m)
{
  memset(m, 0, 10 * sizeof( unsigned short));
}

inline void setMatrix(unsigned short*m, int i, int j, bool v)
{
   m[j] |= 1 << i;
}

static void displayChar(int i, int j, bool v)
{
  TextLayer *layer = qlocktwo_layer[i][j];  
  #ifdef DEBUG
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s %dx%d", ((v) ? "show" : "hide"), i, j);
  #endif
  text_layer_set_font(layer, v ? eq_font : lt_font);
  text_layer_set_text_color(layer, v ? eq_active : eq_text);  
}

inline  void activeChar(unsigned short*m, int x, int y) 
{
  setMatrix(m, x, y, true);
  #ifdef DEBUG
  APP_LOG(APP_LOG_LEVEL_DEBUG, "setting %dx%d : %d", x, y, m[y]);
  #endif
}

static void activateString(unsigned short*m, int hour, int min ) 
{
  switch (min)
  {
  case 0:
    // oclock
    activeChar(m, 5, 9);
    activeChar(m, 6, 9);
    activeChar(m, 7, 9);
    activeChar(m, 8, 9);
    activeChar(m, 9, 9);
    activeChar(m, 10, 9);
    break;
  case 5:
  case 55:
    activeChar(m, 6, 2);
    activeChar(m, 7, 2);
    activeChar(m, 8, 2);
    activeChar(m, 9, 2);
    break;  
  case 10:
  case 50:
    activeChar(m, 5, 3);
    activeChar(m, 6, 3);
    activeChar(m, 7, 3);
    break;
  case 15:
  case 45:
    activeChar(m, 2, 1);
    activeChar(m, 3, 1);
    activeChar(m, 4, 1);
    activeChar(m, 5, 1);
    activeChar(m, 6, 1);
    activeChar(m, 7, 1);
    activeChar(m, 8, 1);
    break;
  case 20:
  case 40:
    activeChar(m, 0, 2);
    activeChar(m, 1, 2);
    activeChar(m, 2, 2);
    activeChar(m, 3, 2);
    activeChar(m, 4, 2);
    activeChar(m, 5, 2);
    break;
  case 25:
  case 35:
    // twenty
    activeChar(m, 0, 2);
    activeChar(m, 1, 2);
    activeChar(m, 2, 2);
    activeChar(m, 3, 2);
    activeChar(m, 4, 2);
    activeChar(m, 5, 2);
    // five
    activeChar(m, 6, 2);
    activeChar(m, 7, 2);
    activeChar(m, 8, 2);
    activeChar(m, 9, 2);
    break;
  case 30:
    // half
    activeChar(m, 0, 3);
    activeChar(m, 1, 3);
    activeChar(m, 2, 3);
    activeChar(m, 3, 3);
    break;
  default:
    break;
  }
  
  if (min != 0)
  {
    // it is
    activeChar(m, 0, 0);
    activeChar(m, 1, 0);
    activeChar(m, 3, 0);
    activeChar(m, 4, 0);    
  }
  
  if (min >1 && min < 31)
  {
    // past
    activeChar(m, 0, 4);
    activeChar(m, 1, 4);
    activeChar(m, 2, 4);
    activeChar(m, 3, 4);
  }
  
  if (min >31 && min < 60)
  {
    // to
    #ifdef DEBUG
    APP_LOG(APP_LOG_LEVEL_DEBUG, "activate to");
    #endif
    activeChar(m, 9, 3);
    activeChar(m, 10, 3);
    hour = (hour + 1) % 12;
  }
  
  switch (hour)
  {
  case 1:
    activeChar(m, 0, 5);
    activeChar(m, 1, 5);
    activeChar(m, 2, 5);
    break;
  case 2:
    activeChar(m, 8, 6);
    activeChar(m, 9, 6);
    activeChar(m, 10, 6);
    break;
  case 3:
    activeChar(m, 6, 5);
    activeChar(m, 7, 5);
    activeChar(m, 8, 5);
    activeChar(m, 9, 5);
    activeChar(m, 10, 5);
    break;
  case 4:
    activeChar(m, 0, 6);
    activeChar(m, 1, 6);
    activeChar(m, 2, 6);
    activeChar(m, 3, 6);
    break;
  case 5:
    activeChar(m, 4, 6);
    activeChar(m, 5, 6);
    activeChar(m, 6, 6);
    activeChar(m, 7, 6);
    break;
  case 6:
    activeChar(m, 3, 5);
    activeChar(m, 4, 5);
    activeChar(m, 5, 5);
    break;
  case 7:
    activeChar(m, 0, 8);
    activeChar(m, 1, 8);
    activeChar(m, 2, 8);
    activeChar(m, 3, 8);
    activeChar(m, 4, 8);
    break;
  case 8:
    activeChar(m, 0, 7);
    activeChar(m, 1, 7);
    activeChar(m, 2, 7);
    activeChar(m, 3, 7);
    activeChar(m, 4, 7);
    break;
  case 9:
    activeChar(m, 7, 4);
    activeChar(m, 8, 4);
    activeChar(m, 9, 4);
    activeChar(m, 10, 4);
    break;
  case 10:
    activeChar(m, 0, 9);
    activeChar(m, 1, 9);
    activeChar(m, 2, 9);
    break;
  case 11:
    activeChar(m, 5, 7);
    activeChar(m, 6, 7);
    activeChar(m, 7, 7);
    activeChar(m, 8, 7);
    activeChar(m, 9, 7);
    activeChar(m, 10, 7);
    break;
  case 0:
    activeChar(m, 5, 8);
    activeChar(m, 6, 8);
    activeChar(m, 7, 8);
    activeChar(m, 8, 8);
    activeChar(m, 9, 8);
    activeChar(m, 10, 8);
    break;
  default:
    break;
  }
}

static void update_time(struct tm* t) {
  /*
  int step_count = 2345;
  if (show_steps) {
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    // Check the metric has data available for today
    HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
    if(mask & HealthServiceAccessibilityMaskAvailable) {
      step_count = (int) health_service_sum_today(metric);
      // Data is available!
      APP_LOG(APP_LOG_LEVEL_INFO, "Steps today: %d", step_count);
    } else {
      // No data recorded yet today
      APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
    }
  }
  */
  
  const int min = (((t->tm_min + 2) % 60) / 5) * 5;
  // early exit to save battery
  // TODO: do I need to check hour ?
  if (prevMin == min)
    return;
  
  const int hour = (t->tm_hour + ((t->tm_min + 2) > 59)) % 12;
  prevMin = min;
  
  unsigned short newMatrix[10];
  resetMatrix(newMatrix);
  activateString(newMatrix, hour, min);
    
  for (int j = 0; j < 10; j++)
  {
    const unsigned short mask = matrix[j] ^ newMatrix[j];
    #ifdef DEBUG
     APP_LOG(APP_LOG_LEVEL_DEBUG, "setting %d : %d ^ %d = %d", i, matrix[i], newMatrix[i], mask);
    #endif

    if (mask > 0)
      for (int i = 0; i < 11; i++)
        if ((mask >> i) & 1)
          displayChar(j,i,!((matrix[j] >> i) & 1));
  }
    
  memcpy(matrix, newMatrix, 10 * sizeof(unsigned short));
}

/*
static void tap_handler(AccelAxisType axis, int32_t direction) {
  if (shake == 0) {
    // do nothing
    return;
  }
  if (shake == 1) {
    // toggle between expression and solution
    clear = !clear;
  }
  // if shake == 2, just create a new expression
  time_t tm = time(NULL);
  struct tm *tms;
  tms = localtime(&tm);
  update_time(tms);
}
*/

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) 
{
  update_time(tick_time);
}

static void window_load(Window *window) {
  prevMin = -1;
  resetMatrix(matrix);
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(window_layer);
  //lt_font = fonts_load_custom_font(fonts_get_system_font(FONT_KEY_GOTHIC_14));
  lt_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HELVETICA_14));
  eq_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HELVETICA_BOLD_14));


#ifdef PBL_COLOR
  eq_bg = GColorBlack/*GColorOxfordBlue*/;
  eq_text = GColorDarkGray;
  eq_active = GColorWhite;
#else
  eq_bg = GColorBlack;
  eq_text = GColorDarkGray;
  eq_active = GColorWhite;
#endif

  window_set_background_color(window, eq_bg);
  const int height = bounds.size.h/10;
  const int width = bounds.size.w/11;
  for (int i = 0; i < 10; i++)
    for (int j = 0; j < 11; j++)
  {
    qlocktwo_layer[i][j] = text_layer_create((GRect) { .origin = { j*width, i*height }, .size = { width, height } });
    text_layer_set_font(qlocktwo_layer[i][j], lt_font);
    strncpy ( &qlocktwoChar[i][j][0], &QLOCKTWO[i][j], 1 );
    text_layer_set_text(qlocktwo_layer[i][j], &qlocktwoChar[i][j][0]);
    text_layer_set_background_color(qlocktwo_layer[i][j], GColorClear);
    text_layer_set_text_alignment(qlocktwo_layer[i][j], GTextAlignmentCenter);
    text_layer_set_text_color(qlocktwo_layer[i][j], eq_text);
    layer_add_child(window_layer, text_layer_get_layer(qlocktwo_layer[i][j]));
  }
  //for (int j = 0; j < 11; j++)
  //  layer_set_hidden(text_layer_get_layer(qlocktwo_layer[10][j]), true);
}

static void window_unload(Window *window) {
  for (int i = 0; i < 10; i++)
    for (int j = 0; j < 11; j++)
      text_layer_destroy(qlocktwo_layer[i][j]);
}

static void prv_unobstructed_did_change(GRect bounds, void *context) {
/*
  layer_set_frame(text_layer_get_layer(hour_layer), (GRect) { .origin = { 0, bounds.size.h/2-62 }, .size = { bounds.size.w, 65 } });
  layer_set_frame(text_layer_get_layer(hour_label_layer), (GRect) { .origin = { 0, bounds.size.h/2-12 }, .size = { bounds.size.w, 15 } });
  layer_set_frame(text_layer_get_layer(min_layer), (GRect) { .origin = { 0, bounds.size.h/2-8 }, .size = { bounds.size.w, 65 } });
  layer_set_frame(text_layer_get_layer(min_label_layer), (GRect) { .origin = { 0, bounds.size.h/2+42 }, .size = { bounds.size.w, 15 } });
  int step_bottom = bounds.size.h/2-62;
  APP_LOG(APP_LOG_LEVEL_INFO, "Step bottom is %d", step_bottom);
  if (step_bottom > STEP_LAYER_HEIGHT) {
    layer_set_frame(text_layer_get_layer(step_layer), (GRect) { .origin = { 0, 1 }, .size = { bounds.size.w, STEP_LAYER_HEIGHT } });
    layer_set_hidden(text_layer_get_layer(step_layer), false);
  }
  else {
    APP_LOG(APP_LOG_LEVEL_INFO, "No room for step count, missing %d px", step_bottom-STEP_LAYER_HEIGHT);
    layer_set_hidden(text_layer_get_layer(step_layer), true);
  }
  */
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Read color preferences
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if(bg_color_t) {
    GColor bg_color = GColorFromHEX(bg_color_t->value->int32);
    //#ifdef DEBUG
     APP_LOG(APP_LOG_LEVEL_DEBUG, "color received %dx%dx%d", bg_color.r, bg_color.g, bg_color.b);
    //#endif
  }
/*
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColor);
  if(fg_color_t) {
    GColor fg_color = GColorFromHEX(fg_color_t->value->int32);
  }

  // Read boolean preferences
  Tuple *second_tick_t = dict_find(iter, MESSAGE_KEY_SecondTick);
  if(second_tick_t) {
    bool second_ticks = second_tick_t->value->int32 == 1;
  }

  Tuple *animations_t = dict_find(iter, MESSAGE_KEY_Animations);
  if(animations_t) {
    bool animations = animations_t->value->int32 == 1;
  }
*/
}

void prv_init(void) {
  // Open AppMessage connection
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
}

static void init(void) {
//  accel_tap_service_subscribe(tap_handler);
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, false /*animated*/);
  UnobstructedAreaHandlers handlers = {
    .will_change = prv_unobstructed_did_change,
  };
  unobstructed_area_service_subscribe(handlers, NULL);
  prv_init();
}

static void deinit(void) {
  unobstructed_area_service_unsubscribe();
  window_destroy(window);
  tick_timer_service_unsubscribe();
//  accel_tap_service_unsubscribe();
}

int main(void) {
  init();
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  app_event_loop();
  deinit();
}

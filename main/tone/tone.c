#include "tone.h"
#include "ledc/ledc.h"
static TaskHandle_t _tone_task = NULL;
static QueueHandle_t _tone_queue = NULL;

static void tone_task(void*){
  tone_msg_t tone_msg;
  while(1){
    xQueueReceive(_tone_queue, &tone_msg, portMAX_DELAY);
    switch(tone_msg.tone_cmd){
      case TONE_START:
        ESP_LOGI(TAG, "Task received from queue TONE_START: frequency=%u Hz, duration=%lu ms", tone_msg.frequency, tone_msg.duration);

        ledc_tone(tone_msg.frequency, tone_msg.duration);
        break;

      case TONE_END:
        ledc_tone(0, 0);
        break;

      default: ; // do nothing
    } // switch
   } // infinite loop
}


int tone_init() {
  if(_tone_queue == NULL){
    ESP_LOGI(TAG, "Creating tone queue");
    _tone_queue = xQueueCreate(128, sizeof(tone_msg_t));
    if(_tone_queue == NULL){
      ESP_LOGE(TAG, "Could not create tone queue");
      return 0; // ERR
    }
    ESP_LOGI(TAG, "Tone queue created");
  }

  if(_tone_task == NULL){
    ESP_LOGI(TAG, "Creating tone task");
    xTaskCreate(
      tone_task, // Function to implement the task
      "toneTask", // Name of the task
      3500,  // Stack size in words
      NULL,  // Task input parameter
      1,  // Priority of the task
      &_tone_task  // Task handle.
      );
    if(_tone_task == NULL){
      ESP_LOGE(TAG, "Could not create tone task");
      return 0; // ERR
    }
    ESP_LOGI(TAG, "Tone task created");
  }
  return 1; // OK
}

void tone_play(unsigned int frequency, unsigned long duration){
    tone_msg_t tone_msg = {
        .tone_cmd = TONE_START,
        .frequency = frequency,
        .duration = duration,
    };
    xQueueSend(_tone_queue, &tone_msg, portMAX_DELAY);
    return;
}

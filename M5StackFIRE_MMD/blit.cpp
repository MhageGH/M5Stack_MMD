//
// blit.cpp
//
#include <M5Stack.h>

#include "blit.h"

struct BlitData
{
  int blitX, blitY, blitW, blitH, idx;
  void *blitBuffer = NULL;
  void *zBuffer = NULL;
};

namespace
{
static int blitIndex = 0;
static QueueHandle_t blitQueue;
} // namespace

namespace blit
{

void Request(int x, int y, int w, int h, void *b, void *z, int idx)
{
  BlitData d;
  d.blitX = x;
  d.blitY = y;
  d.blitW = w;
  d.blitH = h;
  d.blitBuffer = b;
  d.zBuffer = z;
  d.idx = idx;
  if (xQueueSend(blitQueue, &d, portMAX_DELAY) != pdTRUE)
  {
    printf("FAILED QUEUE\n");
  }
  blitIndex++;
}

int GetIndex()
{
  return blitIndex;
}

static void blitTask(void *)
{
  BlitData b;
  while (1)
  {
    if (xQueueReceive(blitQueue, &b, portMAX_DELAY) == pdTRUE)
    {
      //debug_printf("DEBUG : %s : buf=%p\n", __func__, b.blitBuffer);
      uint16_t *src = (uint16_t *)b.blitBuffer;
      for(int i = 0 ; i < b.blitH; i++) {
        M5.Lcd.pushImage(b.blitX, b.blitY + i * b.idx, b.blitW, 1, &src[i * b.blitW]);
      }
    }
  }
}

void Setup()
{
  M5.Lcd.setSwapBytes(true);
  blitQueue = xQueueCreate(BLIT_QUEUE_MAX, sizeof(BlitData));
  xTaskCreate(blitTask, "blitTask", 2048, NULL, 1, NULL);
}

} // namespace blit